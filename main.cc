#include <drogon/drogon.h>

#include <functional>
#include <string>

using namespace drogon;

int main() {
  // Set HTTP listener address and port
  app().addListener("0.0.0.0", 5555);
  // Load config file
  // drogon::app().loadConfigFile("../config.json");
  // drogon::app().loadConfigFile("../config.yaml");

  app().registerHandler(
      "/hello", [](const HttpRequestPtr &req,
                   std::function<void(const HttpResponsePtr &)> &&callback) {
        auto resp = HttpResponse::newHttpResponse();
        resp->setBody("Hello, World!");
        callback(resp);
      });

  // Run HTTP framework,the method will block in the internal event loop
  app().run();
  return 0;
}
