#ifndef MIMES_H
#define MIMES_H

#include <iostream>
#include <unordered_map>

namespace Mime {

    std::unordered_map<std::string, std::string> Mimes = {
        // application
        {"edi-x12", "application/EDI-X12"},
        {"edifact", "application/EDIFACT"},
        {"octet-stream", "application/octet-stream"},
        {"ogg", "application/ogg"},
        {"pdf", "application/pdf"},
        {"xhtml+xml", "application/xhtml+xml"},
        {"x-shockwave-flash", "application/x-shockwave-flash"},
        {"ld+json", "application/ld+json"},
        {"xml", "application/xml"},
        {"zip", "application/zip"},
        {"x-www-form-urlencoded", "application/x-www-form-urlencoded"},
        // audio
        {"mpeg", "audio/mpeg"},
        {"webm", "audio/webm"},
        {"ogg", "audio/ogg"},
        {"wav", "audio/wav"},
        {"x-ms-wma", "audio/x-ms-wma"},
        {"vnd.rn-realaudio", "audio/vnd.rn-realaudio"},
        {"x-wav", "audio/x-wav"},
        // image
        {"gif", "image/gif"},
        {"jpeg", "image/jpeg"},
        {"png", "image/png"},
        {"svg+xml", "image/svg+xml"},
        {"tiff", "image/tiff"},
        {"vnd.microsoft.icon", "image/vnd.microsoft.icon"},
        {"x-icon", "image/x-icon"},
        {"vnd.djvu", "image/vnd.djvu"},
        // multipart
        {"mixed", "multipart/mixed"},
        {"alternative", "multipart/alternative"},
        {"related", "multipart/related"},
        {"form-data", "multipart/form-data"},
        // text
        {"css", "text/css"},
        {"csv", "text/csv"},
        {"html", "text/html"},
        {"javascript", "text/javascript"},
        {"plain", "text/plain"},
        {"xml", "text/xml"},
        // video
        {"mpeg", "video/mpeg"},
        {"mp4", "video/mp4"},
        {"webm", "video/webm"},
        {"x-flv", "video/x-flv"},
        {"quicktime", "video/quicktime"},
        {"x-ms-wmv", "video/x-ms-wmv"},
        {"x-msvideo", "video/x-msvideo"},
        {"3gpp", "video/3gpp"},
        {"3gpp2", "video/3gpp2"},
    };

    std::string get_mime(std::string mime_name){
        return Mimes[mime_name];
    }
};

#endif