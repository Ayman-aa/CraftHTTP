#!/usr/bin/env php-cgi
<?php
// Set the content type of the response to HTML
echo "Content-type: text/html\r\n\r\n";

// Function to recursively list directory contents
function listDirectory($dir, $baseDir) {
    $result = "<ul>";
    $files = scandir($dir);
    foreach ($files as $file) {
        if ($file != "." && $file != "..") {
            $filePath = $dir . DIRECTORY_SEPARATOR . $file;
            $relativePath = str_replace($baseDir, '', $filePath);
            if (is_dir($filePath)) {
                $result .= "<li><strong><a href=\"$relativePath\">$file</a></strong>";
                $result .= listDirectory($filePath, $baseDir);
                $result .= "</li>";
            } else {
                $result .= "<li><a href=\"$relativePath\">$file</a></li>";
            }
        }
    }
    $result .= "</ul>";
    return $result;
}

// Define the directory to list
$directory = __DIR__;
$baseDir = realpath(__DIR__ . '/..'); // Adjust the base directory to the 'assets' directory

// Create an HTML page with the directory structure
echo "<html>\n";
echo "<head>\n";
echo "<title>Directory Structure</title>\n";
echo "<style>\n";
echo "body { font-family: Arial, sans-serif; }\n";
echo "ul { list-style-type: none; }\n";
echo "li { margin: 5px 0; }\n";
echo "a { text-decoration: none; color: #007BFF; }\n";
echo "a:hover { text-decoration: underline; }\n";
echo "</style>\n";
echo "</head>\n";
echo "<body>\n";
echo "<h1>Directory Structure</h1>\n";
echo listDirectory($directory, $baseDir);
echo "</body>\n";
echo "</html>\n";
?>