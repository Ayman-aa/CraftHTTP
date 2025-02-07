#ifndef CONTENT_HPP
#define CONTENT_HPP

#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iomanip>

/**
 * Content class
 *
 * Manages dynamic binary content using a vector of unsigned characters.
 * Provides functionality to append data, search within the content,
 * extract substrings, erase portions of the content, and convert the content to a string.
 */
class Content
{
	private:
		// Vector to store binary data.
		std::vector<unsigned char> data;
	public:
		/**
		 * Default constructor.
		 */
		Content();
		
		/**
		 * Constructs a Content object with initial data.
		 *
		 * @param initialData Vector containing initial binary data.
		 */
		Content(const std::vector<unsigned char>& initialData);
		
		/**
		 * Constructs a Content object from raw data.
		 *
		 * @param rawData Pointer to raw data.
		 * @param size Number of bytes from rawData to initialize content.
		 */
		Content(const unsigned char* rawData, size_t size);

		/**
		 * Appends data from a vector to the current content.
		 *
		 * @param newData Vector containing data to append.
		 */
		void append(const std::vector<unsigned char>& newData);
		
		/**
		 * Appends raw data to the current content.
		 *
		 * @param newData Pointer to the data to append.
		 * @param size Number of bytes to append from newData.
		 */
		void append(const unsigned char* newData, size_t size);
		
		/**
		 * Searches for a substring within the content.
		 *
		 * @param pattern The string pattern to search for.
		 * @return The index position where the pattern is first found, or std::string::npos if not found.
		 */
		size_t find(const std::string& pattern) const;
		
		/**
		 * Extracts a substring from the content.
		 *
		 * @param start Starting index from which to extract.
		 * @param length The number of bytes to extract (default is std::string::npos).
		 * @return A Content object containing the extracted data.
		 */
		Content substr(size_t start, std::string::size_type length = std::string::npos) const;
		
		/**
		 * Returns the size of the content in bytes.
		 *
		 * @return The size of the content.
		 */
		size_t size();
		
		/**
		 * Erases a portion of the content.
		 *
		 * @param start The starting index where erasure begins.
		 * @param length The number of bytes to erase.
		 */
		void erase(size_t start, size_t length);
		
		/**
		 * Converts the content to a string.
		 *
		 * @return A string representation of the binary content.
		 */
		std::string toStr() const;
		
		/**
		 * Assignment operator for Content.
		 *
		 * @param other The Content instance to assign from.
		 * @return Reference to the current Content instance.
		 */
		Content& operator=(const Content& other);
		
		/**
		 * Overloaded stream insertion operator to output the content in hexadecimal format.
		 *
		 * @param os Output stream.
		 * @param Content The Content object to output.
		 * @return Reference to the output stream.
		 */
		friend std::ostream& operator<<(std::ostream& os, const Content& Content);
};

#endif