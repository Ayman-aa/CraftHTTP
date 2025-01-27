#include "../includes/utils.hpp"

bool isValidBase(const std::string& number, unsigned int &size, const int base)
{
    char *endPtr;
    size = std::strtoul(number.c_str(), &endPtr, base);
    if (*endPtr || size == UINT_MAX)
        return false;
    return true;
}



std::string postHtmlResponseGenerator(std::vector<std::string>& filesNames)
{
    std::string htmlResp = ""
    "<!DOCTYPE html>"
    "<html lang=\"en\">"
    "<head>"
        "<meta charset=\"UTF-8\">"
        "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
        "<title>File Upload Success</title>"
        "<style>"
            ":root {"
                "--color-primary: #eee;"
                "--color-inverted: #222;"
            "}"
            "@media (prefers-color-scheme: dark) {"
                ":root {"
                    "--color-primary: #222;"
                    "--color-inverted: #aaa;"
                "}"
            "}"
            "html, body {"
                "margin: 0;"
                "padding: 0;"
                "min-height: 100%;"
                "height: 100%;"
                "width: 100%;"
                "background-color: var(--color-primary);"
                "color: var(--color-inverted);"
                "font-family: monospace;"
                "font-size: 16px;"
                "word-break: keep-all;"
            "}"
            "@media screen and (min-width: 2000px) {"
                "html, body {"
                    "font-size: 20px;"
                "}"
            "}"
            "body {"
                "display: flex;"
                "justify-content: center;"
                "align-items: center;"
            "}"
            "main {"
                "display: flex;"
            "}"
            "article {"
                "display: flex;"
                "align-items: center;"
                "justify-content: center;"
                "flex-direction: column;"
            "}"
            "article #error_text h1 {"
                "font-size: 2em;"
                "font-weight: normal;"
                "padding: 0;"
                "margin: 0;"
            "}"
        "</style>"
    "</head>"
    "<body>"
        "<main>"
            "<article>"
                "<div id=\"error_text\">"
                    "<h1 class=\"source\">File Uploaded Successfully!</h1>"
                    "<h1 class=\"target\"></h1>"
                    "<p>The following files have been successfully uploaded to the server:</p>"
                    "<ul>";
                        for (size_t i = 0; i < filesNames.size(); i++)
                            htmlResp += "<li>" + filesNames[i] + "</li>" ;
                    htmlResp += "</ul>"
                "</div>"
            "</article>"
        "</main>"
    "</body>"
    "</html>";
    return htmlResp;
}

std::string replaceNewlineWithLiteral(const std::string &input)
{
    std::string result = input;
    size_t pos = 0;

    while ((pos = result.find("\r\n", pos)) != std::string::npos)
    {
        result.replace(pos, 2, "\\r\\n");
        pos += 4; // Advance position (skip replaced string)
    }

    return result;
}