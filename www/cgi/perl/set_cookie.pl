#!/bin/perl

use CGI;
$query = new CGI;

$cookie = $query->cookie(-name=>'MY_COOKIE',
			 -value=>'Cookies and sessions are succesfully managed!',
			 -expires=>'+4h',
			 -path=>'/');

print $query->header(-cookie=>$cookie);

print $query->start_html('My cookie-set.cgi program');
print $query->h3('The cookie has been set');
print $query->end_html;

