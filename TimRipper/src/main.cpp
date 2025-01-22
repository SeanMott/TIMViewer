/*
Tim Ripper || based on a Pepsiman extractor || https://github.com/elsemieni/pepsiman-texture-unpacker/tree/master
*/

#define FMT_HEADER_ONLY
#include <fmt/color.h>
#include <fmt/core.h>

#include <filesystem>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

//defines settings and properties
const uint32_t tim_header = 16;
const uint32_t tim_4bpp = 8;
const uint32_t tim_8bpp = 9;
const uint32_t tim_16bpp = 2;
const uint32_t tim_24bpp = 3;

//defines the ripper settings
struct ArgumentSettings
{
	std::filesystem::path bundleFP, outDir;
	size_t maxExtractCount = 512;
};

//------arguments

//sets the filepath to the TIM Bundle
#define TIM_RIPPER_CLI_ARGUMENT_FLAG_STRING_TIM_BUNDLE_FILEPATH "-bundle"

//sets the output directory
#define TIM_RIPPER_CLI_ARGUMENT_FLAG_STRING_RIPPED_TIM_OUTPUT_DIRECTORY_PATH "-out"

//sets the max extracts
#define TIM_RIPPER_CLI_ARGUMENT_FLAG_STRING_RIPPED_TIM_MAX_EXTRACT_COUNT "-maxExtractCount"

//prints help info
static inline void PrintHelpInfo()
{
	fmt::print("Tim Ripper || Rips a TIM Bundle into single TIMs.\n\n"
		"---example useage\nTimRipper.exe {} 3DFX.tim {} TestBundleExtracted\n\n"
		"----CLI FLAGS----\n\n"
		"{} <filepath String> || Required || Sets the TIM Bundle we are gonna rip apart\n\n"
		"{} <directory path String> || Optional || Sets the directory we throw the single TIMs and config file into."
		"If not set, we throw it in the same directory the Bundle was found.\n\n",
		TIM_RIPPER_CLI_ARGUMENT_FLAG_STRING_TIM_BUNDLE_FILEPATH, TIM_RIPPER_CLI_ARGUMENT_FLAG_STRING_RIPPED_TIM_OUTPUT_DIRECTORY_PATH,
		TIM_RIPPER_CLI_ARGUMENT_FLAG_STRING_TIM_BUNDLE_FILEPATH, TIM_RIPPER_CLI_ARGUMENT_FLAG_STRING_RIPPED_TIM_OUTPUT_DIRECTORY_PATH);
}

//parses the arguments
static inline ArgumentSettings ParseArguments(const int argc, char* argv[])
{
	ArgumentSettings settings;


	return settings;
}

//entry point
int main(int argc, char* argv[])
{
	//prints help info
	if (argc < 2)
	{
		PrintHelpInfo();
		getchar();
		return 0;
	}

	//parses the arguments
	const ArgumentSettings settings = ParseArguments(argc, argv);

	FILE* source, * dest, * fileTable;
	uint32_t totalSize, address, checkValue, fileSize;
	char filenameBuffer[256], * fileBuffer;

	std::vector<uint32_t> addresses; addresses.resize(settings.maxExtractCount);
	int i, noOfElements;

	if (argc != 2) {
		printf("Usage: extractor.exe filename\nExample: extractor.exe 0001");
		return -1;
	}

	source = fopen(argv[1], "rb");
	if (!source) {
		printf("Error opening file %s", argv[1]);
		return -1;
	}

	//get file total size
	fseek(source, 0L, SEEK_END);
	totalSize = ftell(source);
	fileBuffer = (char*)malloc(sizeof(char) * totalSize);

	noOfElements = 0;
	address = 0;
	fseek(source, 0, SEEK_SET);
	while (fread(&checkValue, sizeof(uint32_t), 1, source) != 0)
	{
		//if I found a possible tim header, try to next next 32 bit block if there's a valid header.
		if (checkValue == tim_header)
		{
			if (fread(&checkValue, sizeof(uint32_t), 1, source) != 0) {
				if (checkValue == tim_4bpp || checkValue == tim_8bpp || checkValue == tim_16bpp || checkValue == tim_24bpp)
				{
					//found it!
					printf("Found possible TIM file at 0x%.4x\n", address);
					addresses[noOfElements] = address;
					noOfElements++;
				}
			}
			address += sizeof(uint32_t);
		}
		address += sizeof(uint32_t);
	}

	printf("Found %d possible TIM images in this file\n", noOfElements);
	sprintf(filenameBuffer, "%s_filetable.txt", argv[1]);
	fileTable = fopen(filenameBuffer, "w");

	for (i = 0; i < noOfElements; i++) {
		if (i != noOfElements - 1) {
			//read until next register
			fileSize = addresses[i + 1] - addresses[i];

		}
		else {
			//read until EOF
			fileSize = totalSize - addresses[i];
		}

		sprintf(filenameBuffer, "%s_extract_%d.tim", argv[1], i);
		printf("Extracting %s, %d bytes ...\n", filenameBuffer, fileSize);
		fprintf(fileTable, "%d\n", addresses[i]);

		//read packed file from source
		fseek(source, addresses[i], SEEK_SET);
		fread(fileBuffer, fileSize, 1, source);

		//write at destiny file
		dest = fopen(filenameBuffer, "wb");
		if (!dest) {
			printf("Error writing file %s. Did you have write permission here?", filenameBuffer);
			fclose(source);
			return -1;
		}

		fwrite(fileBuffer, fileSize, 1, dest);
		fclose(dest);
	}

	//done
	fclose(fileTable);
	fclose(source);
	printf("%d files processed correctly!\n", noOfElements);

	return 0;

}