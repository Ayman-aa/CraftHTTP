#!/usr/bin/env ruby

require 'cgi'
require 'json'

cgi = CGI.new
action = cgi['action']

# Depending on the action parameter, choose output type and content
if action == "json"
  # Output JSON content with proper content-type header
  data = { message: "Hello from Ruby CGI", time: Time.now.to_s }
  puts cgi.header("type" => "application/json; charset=UTF-8")
  puts data.to_json
  exit
else
  # Default output is HTML
  puts cgi.header("type" => "text/html; charset=UTF-8")
  html = "<html>\n<head>\n<title>Creative Ruby CGI Script</title>\n</head>\n<body>\n"
  
  case action
  when "time"
    html << "<h1>Current Time</h1>\n<p>The current time is: #{Time.now}</p>\n"
  when "random"
    quotes = [
      "The only limit to our realization of tomorrow is our doubts of today.",
      "Stay foolish, stay hungry.",
      "Creativity is intelligence having fun."
    ]
    html << "<h1>Random Quote</h1>\n<blockquote>#{quotes.sample}</blockquote>\n"
  else
    html << "<h1>Welcome!</h1>\n<p>Try adding <code>?action=time</code> to see the current time,<br>"
    html << "or <code>?action=random</code> to see a random quote,<br>"
    html << "or <code>?action=json</code> for a JSON response.</p>\n"
  end

  html << "</body>\n</html>\n"
  puts html
end