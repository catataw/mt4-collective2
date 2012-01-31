/*=============================================================================
	This file is a part of the MetaTrader connector for Collective2 (mt4-collective2).

    mt4-collective2 is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

     mt4-collective2 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with mt4-collective2.  If not, see <http://www.gnu.org/licenses/>.

    Copyright (c) 2010 by OpenThinking Systems, LLC.
=============================================================================*/


#include "HTTPRspCodeDescription.h"

HTTPRspCodeDescription::HTTPRspCodeDescription(void)
{
	m_CodeDescriptionList[ 100 ] = "This interim response is used to inform the client that the initial part of the request has been received and has not yet been rejected by the server.";
	m_CodeDescriptionList[ 101 ] = "The server will switch protocols to those defined by the response's Upgrade header field immediately after the empty line which terminates the 101 response.";
	m_CodeDescriptionList[ 200 ] = "The request has succeeded.";
	m_CodeDescriptionList[ 201 ] = "The request has been fulfilled and resulted in a new resource being created. The newly created resource can be referenced by the URI(s) returned in the entity of the response, with the most specific URI for the resource given by a Location header field.";
	m_CodeDescriptionList[ 202 ] = "The request has been accepted for processing, but the processing has not been completed.  The request might or might not eventually be acted upon, as it might be disallowed when processing actually takes place. There is no facility for re-sending a status code from an asynchronous operation such as this.";
	m_CodeDescriptionList[ 203 ] = "The returned metainformation in the entity-header is not the definitive set as available from the origin server, but is gathered from a local or a third-party copy. The set presented MAY be a subset or superset of the original version.";
	m_CodeDescriptionList[ 204 ] = "The server has fulfilled the request but does not need to return an entity-body, and might want to return updated metainformation. The response MAY include new or updated metainformation in the form of entity-headers, which if present SHOULD be associated with the requested variant.";
	m_CodeDescriptionList[ 205 ] = "The server has fulfilled the request and the user agent SHOULD reset the document view which caused the request to be sent.";
	m_CodeDescriptionList[ 206 ] = "The server has fulfilled the partial GET request for the resource. The request MUST have included a Range header field indicating the desired range, and MAY have included an If-Range header field to make the request conditional.";
	m_CodeDescriptionList[ 300 ] = "The requested resource corresponds to any one of a set of representations, each with its own specific location, and agent-driven negotiation information is being provided so that the user (or user agent) can select a preferred representation and redirect its request to that location.";
	m_CodeDescriptionList[ 301 ] = "The requested resource has been assigned a new permanent URI and any future references to this resource SHOULD use one of the returned URIs.";
	m_CodeDescriptionList[ 302 ] = "The requested resource resides temporarily under a different URI. Since the redirection might be altered on occasion, the client SHOULD continue to use the Request-URI for future requests.";
	m_CodeDescriptionList[ 303 ] = "The response to the request can be found under a different URI and SHOULD be retrieved using a GET method on that resource.";
	m_CodeDescriptionList[ 304 ] = "If the client has performed a conditional GET request and access is allowed, but the document has not been modified, the server SHOULD respond with this status code.";
	m_CodeDescriptionList[ 305 ] = "The requested resource MUST be accessed through the proxy given by the Location field. The Location field gives the URI of the proxy. The recipient is expected to repeat this single request via the proxy.";
	m_CodeDescriptionList[ 306 ] = "The 306 status code was used in a previous version of the specification, is no longer used, and the code is reserved.";
	m_CodeDescriptionList[ 307 ] = "The requested resource resides temporarily under a different URI. Since the redirection MAY be altered on occasion, the client SHOULD continue to use the Request-URI for future requests.";
	m_CodeDescriptionList[ 400 ] = "The request could not be understood by the server due to malformed syntax. The client SHOULD NOT repeat the request without modifications.";
	m_CodeDescriptionList[ 401 ] = "The request requires user authentication.";
	m_CodeDescriptionList[ 402 ] = "This code is reserved for future use.";
	m_CodeDescriptionList[ 403 ] = "The server understood the request, but is refusing to fulfill it. Authorization will not help and the request SHOULD NOT be repeated.";
	m_CodeDescriptionList[ 404 ] = "The server has not found anything matching the Request-URI. No indication is given of whether the condition is temporary or permanent.";
	m_CodeDescriptionList[ 405 ] = "The method specified in the Request-Line is not allowed for the resource identified by the Request-URI.";
	m_CodeDescriptionList[ 406 ] = "The resource identified by the request is only capable of generating response entities which have content characteristics not acceptable according to the accept headers sent in the request.";
	m_CodeDescriptionList[ 407 ] = "This code is similar to 401 (Unauthorized), but indicates that the client must first authenticate itself with the proxy.";
	m_CodeDescriptionList[ 408 ] = "The client did not produce a request within the time that the server was prepared to wait. The client MAY repeat the request without modifications at any later time.";
	m_CodeDescriptionList[ 409 ] = "The request could not be completed due to a conflict with the current state of the resource.";
	m_CodeDescriptionList[ 410 ] = "The requested resource is no longer available at the server and no forwarding address is known.";
	m_CodeDescriptionList[ 411 ] = "The server refuses to accept the request without a defined Content-Length.";
	m_CodeDescriptionList[ 412 ] = "The precondition given in one or more of the request-header fields evaluated to false when it was tested on the server.";
	m_CodeDescriptionList[ 413 ] = "The server is refusing to process a request because the request entity is larger than the server is willing or able to process.";
	m_CodeDescriptionList[ 414 ] = "The server is refusing to service the request because the Request-URI is longer than the server is willing to interpret.";
	m_CodeDescriptionList[ 416 ] = "A server SHOULD return a response with this status code if a request included a Range request-header field, and none of the range-specifier values in this field overlap the current extent of the selected resource, and the request did not include an If-Range request-header field.";
	m_CodeDescriptionList[ 417 ] = "The expectation given in an Expect request-header field could not be met by this server, or, if the server is a proxy, the server has unambiguous evidence that the request could not be met by the next-hop server.";
	m_CodeDescriptionList[ 415 ] = "The server is refusing to service the request because the entity of the request is in a format not supported by the requested resource for the requested method.";
	m_CodeDescriptionList[ 500 ] = "he server encountered an unexpected condition which prevented it from fulfilling the request.";
	m_CodeDescriptionList[ 501 ] = "The server does not support the functionality required to fulfill the request.";
	m_CodeDescriptionList[ 502 ] = "The server, while acting as a gateway or proxy, received an invalid response from the upstream server it accessed in attempting to fulfill the request.";
	m_CodeDescriptionList[ 503 ] = "The server is currently unable to handle the request due to a temporary overloading or maintenance of the server.";
	m_CodeDescriptionList[ 504 ] = "The server, while acting as a gateway or proxy, did not receive a timely response from the upstream server specified by the URI (e.g. HTTP, FTP, LDAP) or some other auxiliary server (e.g. DNS) it needed to access in attempting to complete the request.";
	m_CodeDescriptionList[ 505 ] = "he server does not support, or refuses to support, the HTTP protocol version that was used in the request message.";
}

std::string HTTPRspCodeDescription::getInfo( unsigned int pResponseCode ){
	std::map<unsigned int, std::string>::iterator codeDescrItr = m_CodeDescriptionList.find( pResponseCode );
	if ( codeDescrItr != m_CodeDescriptionList.end() ) {
		return codeDescrItr->second;
	}
	return "UNKNOWN RESPONSE CODE";

}
