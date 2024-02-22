#ifndef UTILS_NETWORK_H
#define UTILS_NETWORK_H

#include <Magnum/Trade/ImageData.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Containers/StringView.h>
#include <Magnum/Trade/AbstractImporter.h>
#include <cpr/cpr.h>
#include <iostream>
#include <fstream>

using namespace Magnum;

static Containers::Optional<Trade::ImageData2D> loadImageFromUrl(const Corrade::Containers::StringView& url) {
  // 下载图像数据
  cpr::Response response = cpr::Get(cpr::Url{url});
  if (response.status_code != 200) {
    return {};  // 下载失败
  }

  // 将图像数据解码为像素数据
  PluginManager::Manager<Trade::AbstractImporter> manager;
  Containers::Pointer<Trade::AbstractImporter> importer = manager.loadAndInstantiate("AnyImageImporter");
  if (!importer->openData({response.text.data(), std::size_t(response.text.size())})) {
    return {};  // 解码失败
  }

  return importer->image2D(0);
}

static void downloadVideo(const std::string& url, const std::string& outputFile) {
  // 下载视频数据
  cpr::Response response = cpr::Get(cpr::Url{url});
  if (response.status_code != 200) {
    std::cerr << "Failed to download video data" << std::endl;
    exit(1);
  }
  std::ofstream outfile(outputFile, std::ofstream::binary);
  outfile.write(response.text.data(), response.text.size());
}

#endif
