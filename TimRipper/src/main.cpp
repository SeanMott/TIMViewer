/*
Tim Ripper || based on a Pepsiman extractor || https://github.com/elsemieni/pepsiman-texture-unpacker/tree/master
*/

#define FMT_HEADER_ONLY
#include <fmt/color.h>
#include <fmt/core.h>

#include <filesystem>

#include <vector>

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
		"If not set, we throw it in the same directory the Bundle was found.\n\n"
		"{} <Integer> || Optional || Sets the max number of TIMs to extract, if not set, defaults to 512.\n\n",
		TIM_RIPPER_CLI_ARGUMENT_FLAG_STRING_TIM_BUNDLE_FILEPATH, TIM_RIPPER_CLI_ARGUMENT_FLAG_STRING_RIPPED_TIM_OUTPUT_DIRECTORY_PATH,
		TIM_RIPPER_CLI_ARGUMENT_FLAG_STRING_TIM_BUNDLE_FILEPATH, TIM_RIPPER_CLI_ARGUMENT_FLAG_STRING_RIPPED_TIM_OUTPUT_DIRECTORY_PATH,
		TIM_RIPPER_CLI_ARGUMENT_FLAG_STRING_RIPPED_TIM_MAX_EXTRACT_COUNT);
}

//parses the arguments
static inline ArgumentSettings ParseArguments(const int argc, char* argv[])
{
	ArgumentSettings settings;

	//parse the arguments
	for (size_t i = 1; i < argc; ++i)
	{
		//bundle path
		if (!strcmp(argv[i], TIM_RIPPER_CLI_ARGUMENT_FLAG_STRING_TIM_BUNDLE_FILEPATH))
		{
			//checks for value

			//gets value
			i++;
			settings.bundleFP = argv[i];
		}

		//output directory
		else if (!strcmp(argv[i], TIM_RIPPER_CLI_ARGUMENT_FLAG_STRING_RIPPED_TIM_OUTPUT_DIRECTORY_PATH))
		{
			//checks for value

			//gets value
			i++;
			settings.outDir = argv[i];

			//checks if directory ends with /
			if (settings.outDir.string()[settings.outDir.string().size() - 1] != '/')
				settings.outDir += '/';
		}

		//max extracts
		else if (!strcmp(argv[i], TIM_RIPPER_CLI_ARGUMENT_FLAG_STRING_RIPPED_TIM_MAX_EXTRACT_COUNT))
		{
			//checks for value

			//gets value
			i++;
			settings.maxExtractCount = std::stoul(argv[i]);
		}
	}

	return settings;
}

//defines a single TIM in the Bundle
struct ExtractedTIM
{
	uint32_t address = 0x0000; //the address we found the TIM at
};

//defines a TIM Bundle


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

	//sets up data
	uint32_t checkValue, fileSize;
	char filenameBuffer[256];
	const size_t UINT32_MEM_SIZE = sizeof(uint32_t);

	std::vector<ExtractedTIM> extractedTIMs; extractedTIMs.reserve(settings.maxExtractCount);

	//loads bundle
	FILE* source = fopen(settings.bundleFP.string().c_str(), "rb");
	if (!source) 
	{
		fmt::print("Error opening file at \"{}\"\n", settings.bundleFP.string());
		getchar();
		return -1;
	}

	//get file total size
	fseek(source, 0L, SEEK_END);
	const uint32_t totalSize = ftell(source);
	char* fileBuffer = (char*)malloc(sizeof(char) * totalSize);

	uint32_t address = 0;
	fseek(source, 0, SEEK_SET);
	while (fread(&checkValue, UINT32_MEM_SIZE, 1, source) != 0)
	{
		//if I found a possible tim header, try to next next 32 bit block if there's a valid header.
		if (checkValue == tim_header)
		{
			if (fread(&checkValue, UINT32_MEM_SIZE, 1, source) != 0)
			{
				if (checkValue == tim_4bpp || checkValue == tim_8bpp || checkValue == tim_16bpp || checkValue == tim_24bpp)
				{
					//found it!
					printf("Found possible TIM file at 0x%.4x\n", address);
					ExtractedTIM* TIM = &extractedTIMs.emplace_back(ExtractedTIM());
					TIM->address = address;
				}
			}
			address += UINT32_MEM_SIZE;
		}
		address += UINT32_MEM_SIZE;
	}

	const size_t extractedTIMCount = extractedTIMs.size();
	fmt::print("Found {} possible TIM images in this file\n", extractedTIMCount);
	
	//writes a config file
	const std::string configFP = settings.outDir.string() + "/config.txt";
	FILE* fileTable = fopen(configFP.c_str(), "w");
	for (size_t i = 0; i < extractedTIMCount; ++i)
	{
		if (i != extractedTIMCount - 1) //read until next register
			fileSize = extractedTIMs[i + 1].address - extractedTIMs[i].address;
		else //read until EOF
			fileSize = totalSize - extractedTIMs[i].address;

		//writes the extracted TIM file
		const std::string extractedTIMFP = settings.outDir.string() + "TIM_" + std::to_string(i) + ".tim";
		fmt::print("Extracting {}, {} bytes ...\n", extractedTIMFP, fileSize);
		fprintf(fileTable, "%d\n", extractedTIMs[i].address);

		//read packed file from source
		fseek(source, extractedTIMs[i].address, SEEK_SET);
		fread(fileBuffer, fileSize, 1, source);

		//write at destiny file
		FILE* dest = fopen(extractedTIMFP.c_str(), "wb");
		if (!dest)
		{
			fmt::print("Error writing file {}. Did you have write permission here?\n", extractedTIMFP);
			fclose(source);
			return -1;
		}

		fwrite(fileBuffer, fileSize, 1, dest);
		fclose(dest);
	}

	//done
	fclose(fileTable);
	fclose(source);
	fmt::print("{} files processed correctly!\n", extractedTIMCount);

	return 0;

}