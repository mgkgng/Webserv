#!/bin/perl

use CGI;
$query = new CGI;

print $query->header;

print $query->start_html('My cookie-get.cgi program');

print $query->h3('The cookie is ...');
$theCookie = $query->cookie('MY_COOKIE');

print "

    \n"; print $theCookie; print "

\n";

print $query->end_html;