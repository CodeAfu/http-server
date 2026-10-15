#pragma once

#include <stdint.h>
#include <stddef.h>

#define MAX_HEADERS 32

typedef struct {
    char* name;
    char* value;
} HttpHeader;

typedef struct {
    char *buffer; // all request data

    char* method; // "GET"
    char* target; // "/index.html"
    char* version; // "HTTP/1.1"

    HttpHeader headers[MAX_HEADERS];
    size_t header_count;
    
    char* body;
    size_t body_len;
} HttpRequest;

typedef struct {
    uint16_t status; // 200, 404
    const char* reason; // "OK", "Not Found"
    const char* version; // "HTTP/1.1"

    HttpHeader header[MAX_HEADERS];
    size_t header_count;

    const char* body;
    size_t body_len;
} HttpResponse;
