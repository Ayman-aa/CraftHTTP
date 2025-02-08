<?php
header("Content-Type: text/html; charset=UTF-8");

// Cookie name for session
$sessionCookie = 'SESSIONID';

// Check if the SESSIONID cookie exists
if (isset($_COOKIE[$sessionCookie])) {
    $sessionId = $_COOKIE[$sessionCookie];
} else {
    // Generate a new session ID (using random bytes for demonstration)
    $sessionId = bin2hex(random_bytes(16));
    // Set the cookie for one hour, available across the site (path = "/")
    setcookie($sessionCookie, $sessionId, time() + 3600, "/");
}

// Simulated session storage (in real apps, use a persistent storage)
$sessionData = [
    "session1" => ["username" => "JohnDoe"],
    "session2" => ["username" => "JaneDoe"]
];

// Retrieve session data using the session ID
$username = "Guest";
if (isset($sessionData[$sessionId])) {
    $username = $sessionData[$sessionId]["username"];
}

// Output the HTML content
echo "<html><body>";
echo "<h1>Hello, {$username}!</h1>";
echo "<p>Your session ID is: {$sessionId}</p>";
echo "</body></html>";
?>