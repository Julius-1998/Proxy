#ifndef __HTTPRESPONSE_HPP__
#define __HTTPRESPONSE_HPP__

class HttpResponse {
public:
	enum STATUS { OK, REDIRECT, NOT_FOUND, INTERNAL_ERROR };
    STATUS status;
    HttpResponse() : status(OK) { }	
};


#endif
