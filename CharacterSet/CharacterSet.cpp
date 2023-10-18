#define BOM_MAX_LEN 5

const char *BOM_Name(FILE *f) {
	static const struct BOM {
		char *name;
		size_t length;
		unsigned char signature[BOM_MAX_LEN];
	} BOM[] = {                                        // Various UTF encodings
		{ "UTF8", 3, { 0xEF, 0xBB, 0xBF } },           // UTF8
		{ "UTF16BE", 2, { 0xFF, 0xFE } },              // UTF16 Big Endian
		{ "UTF16LE", 2, { 0xFE, 0xFF } },              // UTF16 Little Endian
		{ "UTF32BE", 4, { 0x00, 0x00, 0xFE, 0xFF } },  // UTF32 Big Endian
		{ "UTF32LE", 4, { 0xFF, 0xFE, 0x00, 0x00 } },  // UTF32 Little Endian
													   // Add others as desired.  https://en.wikipedia.org/wiki/Byte_order_mark
		{ NULL, 0, { 0 } } 
	};

	unsigned char BOM_Signature[BOM_MAX_LEN];
	rewind(f);  // Only file beginning
	size_t length = fread(BOM_Signature, 1, BOM_MAX_LEN, f);

	for (size_t i = 0; BOM[i].length; i++) {
		if (length >= BOM[i].length 
				&& memcmp(BOM_Signature, BOM[i].signature, BOM[i].length) == 0) {
			fseek(f, BOM[i].length, SEEK_SET);  // Leave file position to just after BOM
			return BOM[i].name;
		}
	}
	return NULL;
}
