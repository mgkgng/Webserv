#!/bin/perl

use CGI;
$query = new CGI;

print $query->header;

print $query->start_html('My cookie-get.cgi program');

print $query->h3('The cookie is ...');
$theCookie = $query->cookie('NO_COOKIE');
                          
if ("$theCookie") {
   print $theCookie;
} else {
   print "Can't find the cookie!\n";
}

print $query->end_html;