#ifndef HELPERS_VIDEO_H
#define HELPERS_VIDEO_H

#include <cstdio>
#include <vector>
#include "../utils/network.h"
#include "../utils/common.h"
#include <chrono>

extern "C" {
  #include <libavcodec/avcodec.h>
  #include <libavformat/avformat.h>
  #include <libswscale/swscale.h>
  #include <libavutil/imgutils.h>
}

static Int count = 0;

class Video {
  public:
    explicit Video(const std::string& url): _videoPath("tmp/" + generateGuid()) {
      downloadVideo(url, _videoPath);
      init();
      getBasicInfo();
    }

    ~Video() {
      if (_sws_ctx) {
        sws_freeContext(_sws_ctx);
      }
      if (_codec_ctx) {
        avcodec_close(_codec_ctx);
        avcodec_free_context(&_codec_ctx);
      }
      if (_format_ctx) { // 关闭输入流
        avformat_close_input(&_format_ctx);
      }
      // 删除视频
      std::remove(_videoPath.c_str());
    }

    Int Width() {
      return _width;
    }

    Int Height() {
      return _height;
    }

    Int FrameRate() {
      return _frameRate;
    }

    Double Duration() {
      return _duration;
    }

    int saveAsImage(AVFrame *frameRGB) {
      const AVCodec *jpegCodec = avcodec_find_encoder(AV_CODEC_ID_MJPEG);
      if (!jpegCodec) {
          std::cerr << "JPEG codec not found\n";
          return 1;
      }

      AVCodecContext *jpegContext = avcodec_alloc_context3(jpegCodec);
      if (!jpegContext) {
          std::cerr << "Could not allocate JPEG codec context\n";
          return 1;
      }

      jpegContext->pix_fmt = AV_PIX_FMT_YUVJ420P;
      jpegContext->height = frameRGB->height;
      jpegContext->width = frameRGB->width;
      jpegContext->time_base = (AVRational){1, 25};

      if (avcodec_open2(jpegContext, jpegCodec, nullptr) < 0) {
          std::cerr << "Could not open JPEG codec\n";
          return 1;
      }

      AVPacket packet;
      av_init_packet(&packet);
      packet.data = nullptr;
      packet.size = 0;

      int ret = avcodec_send_frame(jpegContext, frameRGB);
      if (ret < 0) {
          std::cerr << "Error sending frame to JPEG codec\n";
          return 1;
      }

      ret = avcodec_receive_packet(jpegContext, &packet);
      if (ret < 0) {
          std::cerr << "Error receiving packet from JPEG codec\n";
          return 1;
      }
      std::string path = "tmp/"+generateGuid()+".jpg";
      FILE *jpegFile = fopen(path.c_str(), "wb");
      if (!jpegFile) {
          std::cerr << "Could not open output file\n";
          return 1;
      }

      fwrite(packet.data, 1, packet.size, jpegFile);
      fclose(jpegFile);

      av_packet_unref(&packet);
      avcodec_free_context(&jpegContext);
      return 0;
    }

    std::vector<uint8_t> getVideoFrameAtTime(Double timeInSeconds) {
      AVStream *video_stream = _format_ctx->streams[_videoStreamIndex];
      AVPacket packet;
      AVFrame *frame = av_frame_alloc();
      AVFrame *frameRGB = av_frame_alloc();

      int64_t timestamp = timeInSeconds * AV_TIME_BASE;
      int64_t frame_pts = av_rescale_q(timestamp, AV_TIME_BASE_Q, video_stream->time_base);

      int width = _codec_ctx->width;
      int height = _codec_ctx->height;

      int num_bytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, width, height, 1);
      std::vector<uint8_t> buffer(num_bytes);

      av_image_fill_arrays(frameRGB->data, frameRGB->linesize, buffer.data(), AV_PIX_FMT_RGB24, width, height, 1);

      av_seek_frame(_format_ctx, _videoStreamIndex, frame_pts, AVSEEK_FLAG_BACKWARD);
      auto start = std::chrono::high_resolution_clock::now();
      while (av_read_frame(_format_ctx, &packet) >= 0) {
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = end - start;
        // if (diff.count() >= 0.01) {
          count += 1;
          // std::cout << count << std::endl;
        // }
        // std::cout << "while executed in " << diff.count() << " seconds\n" << std::endl;
        if (packet.stream_index == _videoStreamIndex) {
          if (avcodec_send_packet(_codec_ctx, &packet) < 0) {
            std::cerr << "Error sending packet for decoding\n";
            exit(1);
          }
          
          bool finishedDecode = false;
          while (avcodec_receive_frame(_codec_ctx, frame) == 0) {
            if (frame->best_effort_timestamp >= frame_pts) {
              // Convert the image from its native format to RGB
              // SwsContext *sws_ctx = sws_getContext(width, height, _codec_ctx->pix_fmt, width, height, AV_PIX_FMT_RGB24, SWS_BILINEAR, nullptr, nullptr, nullptr);
              sws_scale(_sws_ctx, frame->data, frame->linesize, 0, height, frameRGB->data, frameRGB->linesize);
              av_image_copy_to_buffer(buffer.data(), num_bytes, frameRGB->data, frameRGB->linesize, AV_PIX_FMT_RGB24, width, height, 1);
              finishedDecode = true;
              break;
            }
          }
          
          if (finishedDecode) {
            break;
          }
        }
        av_packet_unref(&packet);
      }

      // saveAsImage(frame);

      av_frame_free(&frame);
      av_frame_free(&frameRGB);

      return buffer;
    }

  private:
    Int _width;
    Int _height;
    Int _frameRate;
    Int _videoStreamIndex = -1;
    Double _duration;
    std::string _videoPath;
    AVFormatContext *_format_ctx = nullptr;
    AVCodecContext *_codec_ctx = nullptr;
    SwsContext *_sws_ctx = nullptr;

    void init() {
      if (avformat_open_input(&_format_ctx, _videoPath.c_str(), nullptr, nullptr) != 0) {
        std::cerr << "Error opening input file\n";
        exit(1);
      }
      if (avformat_find_stream_info(_format_ctx, nullptr) < 0) {
        std::cerr << "Error finding stream information\n";
        exit(1);
      }
      for (unsigned int i = 0; i < _format_ctx->nb_streams; ++i) {
        if (_format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
          _videoStreamIndex = i;
          break;
        }
      }
      if (_videoStreamIndex == -1) {
        std::cerr << "Error finding video stream\n";
        exit(1);
      }
      AVStream *video_stream = _format_ctx->streams[_videoStreamIndex];
      AVCodecParameters *codecpar = video_stream->codecpar;

      const AVCodec *codec = avcodec_find_decoder(codecpar->codec_id);
      if (!codec) {
        std::cerr << "Unsupported codec\n";
        exit(1);
      }
      _codec_ctx = avcodec_alloc_context3(codec);
      if (!_codec_ctx) {
        std::cerr << "Could not allocate video codec context\n";
        exit(1);
      }
      _codec_ctx->thread_count = 0;
      _codec_ctx->thread_type = FF_THREAD_FRAME;
      // _codec_ctx->lowres = 1;
      // _codec_ctx->flags2 |= AV_CODEC_FLAG2_FAST;
      // _codec_ctx->refs = 1;
      avcodec_parameters_to_context(_codec_ctx, codecpar);
      if (avcodec_open2(_codec_ctx, codec, nullptr) < 0) {
        std::cerr << "Error opening codec\n";
        exit(1);
      }
      _sws_ctx = sws_getContext(codecpar->width, codecpar->height, _codec_ctx->pix_fmt, codecpar->width, codecpar->height, AV_PIX_FMT_RGB24, SWS_BILINEAR, nullptr, nullptr, nullptr);
    }

    void getBasicInfo() {
      AVCodecParameters *video_codecpar = _format_ctx->streams[_videoStreamIndex]->codecpar;
      _width = video_codecpar->width;
      _height = video_codecpar->height;
      _frameRate = video_codecpar->framerate.num / video_codecpar->framerate.den;
      _duration = _format_ctx->duration / AV_TIME_BASE;
    }
};

#endif