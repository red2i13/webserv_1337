<?php
// Simple PHP script to test CGI handling

echo "Content-Type: text/html\n\n";  // Important for CGI response header

echo "<html>";
echo "<head><title>CGI Test</title></head>";
echo "<body>";
echo "<h1>PHP CGI Test Successful!</h1>";
echo "<p>If you see this, your CGI handler is working.</p>";

// Display some server info for debugging
echo "<pre>";
print_r($_SERVER);
echo "</pre>";

echo "</body>";
echo "</html>";
?>
