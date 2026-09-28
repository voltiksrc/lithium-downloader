#include <cstdio>
#include <curl/curl.h>
#include <fstream>
#include <iostream>

using std::string;

int last_percentage = -1;

int progress_callback(void *clientp, curl_off_t dltotal, curl_off_t dlnow,
                      curl_off_t ultotal, curl_off_t ulnow) {
  if (dltotal > 0) {
    int percentage;
    const int bar_width = 20;
    int filled;
    // stupid progress bar that needs a nested if and 2 for loops
    percentage = static_cast<double>(dlnow) / dltotal * 100;
    if (percentage != last_percentage) {
      filled = percentage * bar_width / 100;
      std::cout << " [";
      for (int i = 0; i < filled; i++) {
        std::cout << "#";
      }
      for (int i = filled; i < bar_width; i++) {
        std::cout << "-";
      }
      std::cout << "]";
      std::cout << " " << percentage << "%\r" << std::flush;
      last_percentage = percentage;
    }
  }
  return 0;
}

// data writing function
size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata) {
  size_t total = size * nmemb;

  std::ofstream *file = static_cast<std::ofstream *>(userdata);

  file->write(ptr, total);

  return total;
}

int main(int argc, char *argv[]) {
  // validation
  if (argc < 2) {
    std::cout << "please enter a url.\n";
    return 1;
  }
  string url = argv[1];
  auto pos = url.find_last_of('/');
  string filename = url.substr(pos + 1);
  if (filename.empty()) {
    std::cerr << "Couldn't determine filename.\n";
    return 1;
  }
  auto query_pos = filename.find('?');
  if (query_pos != std::string::npos) {
    filename = filename.substr(0, query_pos);
  }
  std::ofstream file(filename, std::ios::binary);

  if (!file) {
    std::cerr << "Failed to open output file\n";
    return 1;
  }

  CURL *curl = curl_easy_init();

  // libcurl options
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &file);

    curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, progress_callback);

    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);

    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    CURLcode result = curl_easy_perform(curl);
    std::cout << '\n';
    // more checks
    if (result != CURLE_OK) {
      std::cerr << curl_easy_strerror(result) << '\n';
      file.close();
      std::remove(filename.c_str());
    } else {
      long response_code;
      curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
      if (response_code >= 400) {
        std::cout << "HTTP error: " << response_code << '\n';
        file.close();
        std::remove(filename.c_str());
      }
    }
    curl_easy_cleanup(curl);
  }
}
