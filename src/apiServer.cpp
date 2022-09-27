#include "pch.h"
#include <httplib/httplib.h>
#include "conf.h"

int startApiServer() {
    httplib::Server svr;

    svr.Get("/api/info", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("LMMAP v0.3.3", "text/plain");
    });

    svr.Get("/api/getPlayerMarkers", [](const httplib::Request&, httplib::Response& res) {
        res.set_content(getPlayerMarkers(), "application/json");
    });

    svr.Get("/api/getCustomMarkers", [](const httplib::Request&, httplib::Response& res) {
        res.set_content(getCustomMarkers(), "application/json");
    });

    svr.Get("/api/reloadCustomMarkers", [](const httplib::Request&, httplib::Response& res) {
        loadCustomMarkers();
    });

    svr.listen("0.0.0.0", config.apiServerPort);
    return 0;
}