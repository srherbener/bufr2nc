/*
 * (C) Copyright 2020 NOAA/NWS/NCEP/EMC
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <time.h>

#include <iomanip>
#include <iostream>

#include "eckit/config/YAMLConfiguration.h"
#include "eckit/filesystem/PathName.h"
#include "oops/util/IntSetParser.h"

#include "BufrParser/BufrDescription.h"
#include "BufrParser/BufrMnemonicSet.h"
#include "BufrParser/BufrParser.h"
#include "IodaEncoder/IodaDescription.h"
#include "IodaEncoder/IodaEncoder.h"
#include "DataContainer.h"


static const char* CONFIG_FILE =
    "/Users/rmclaren/Work/ioda-converters/src/bufr/test_ingester.yaml";
static const char* INPUT_FILE =
    "/Users/rmclaren/Work/sample-bufr-data/gdas/gdas.20200704/12/gdas.t12z.1bmhs.tm00.bufr_d";
static const char* OUTPUT_FILE = "/Users/rmclaren/Temp/ioda_encoder_result.nc";


namespace Ingester
{
    void test_createDescriptionManually()
    {
        // Create Description
        auto description = BufrDescription();

        description.setFilepath(INPUT_FILE);

        std::vector<std::string> set1Mnemonics = {"SAID", "FOVN", "YEAR", "MNTH", "DAYS", "HOUR",
                                                  "MINU", "SECO", "CLAT", "CLON", "CLATH", "CLONH",
                                                  "HOLS"};
        std::vector<std::string> set2Mnemonics = {"SAZA", "SOZA", "BEARAZ", "SOLAZI"};
        std::vector<std::string> set3Mnemonics = {"TMBR"};

        auto set1 = BufrMnemonicSet(set1Mnemonics, {1});
        auto set2 = BufrMnemonicSet(set2Mnemonics, {1});
        auto set3 = BufrMnemonicSet(set3Mnemonics, oops::parseIntSet("1-15"));

        description.addMnemonicSet(set1);
        description.addMnemonicSet(set2);
        description.addMnemonicSet(set3);

        // Read some data
        auto bufrParser = BufrParser(description);
        auto data = bufrParser.parse(4);

        data->get("radiance")->print();
    }

    void test_parsePartialFile()
    {
        std::unique_ptr<eckit::YAMLConfiguration>
            yaml(new eckit::YAMLConfiguration(eckit::PathName(CONFIG_FILE)));

        auto inputPath = yaml->getString("inputpath");
        auto outputPath = yaml->getString("outputpath");

        if (yaml->has("bufr"))
        {
            auto conf = yaml->getSubConfiguration("bufr");
            auto bufrDesc = BufrDescription(conf, inputPath);
            auto bufrParser = BufrParser(bufrDesc);

            std::shared_ptr<DataContainer> data = bufrParser.parse(5);

            data->get("timestamp")->print();
        }
    }

    void test_parseWholeFile()
    {
        std::unique_ptr<eckit::YAMLConfiguration>
            yaml(new eckit::YAMLConfiguration(eckit::PathName(CONFIG_FILE)));

        auto inputPath = yaml->getString("inputpath");
        auto outputPath = yaml->getString("outputpath");

        if (yaml->has("bufr"))
        {
            auto conf = yaml->getSubConfiguration("bufr");
            auto bufrDesc = BufrDescription(conf, inputPath);
            auto bufrParser = BufrParser(bufrDesc);

            std::shared_ptr<DataContainer> data = bufrParser.parse();
        }
    }

    void test_parseFileIncrementally()
    {
        std::unique_ptr<eckit::YAMLConfiguration>
            yaml(new eckit::YAMLConfiguration(eckit::PathName(CONFIG_FILE)));

        auto inputPath = yaml->getString("inputpath");
        auto outputPath = yaml->getString("outputpath");

        if (yaml->has("bufr"))
        {
            auto conf = yaml->getSubConfiguration("bufr");
            auto bufrDesc = BufrDescription(conf, inputPath);
            auto bufrParser = BufrParser(bufrDesc);

            bool endReached = false;
            std::shared_ptr<DataContainer> data;
            do
            {
                auto nextData = bufrParser.parse(10);

                if (nextData->size() > 0)
                {
                    data = nextData;
                }
                else
                {
                    endReached = true;
                }
            } while (!endReached);

            data->get("radiance")->print();
        }
    }

    void test_parseFileWEncoder()
    {
        std::unique_ptr<eckit::YAMLConfiguration>
            yaml(new eckit::YAMLConfiguration(eckit::PathName(CONFIG_FILE)));

        auto inputPath = yaml->getString("inputpath");
        auto outputPath = yaml->getString("outputpath");

        if (yaml->has("bufr"))
        {
            auto conf = yaml->getSubConfiguration("bufr");
            auto bufrDesc = BufrDescription(conf, inputPath);
            auto bufrParser = BufrParser(bufrDesc);

            std::shared_ptr<DataContainer> data = bufrParser.parse();

            if (yaml->has("ioda"))
            {
                auto iodaDesc = IodaDescription(yaml->getSubConfiguration("ioda"),
                                                outputPath);
                auto encoder = IodaEncoder(iodaDesc);
                encoder.encode(data);
            }
        }
    }
}  // namespace Ingester

int main(int, const char **)
{
    clock_t startTime = clock();
    Ingester::test_parseFileWEncoder();

    std::cout << "Took " \
              << std::setprecision(2) \
              << (clock() - startTime) / CLOCKS_PER_SEC \
              << " seconds to run." << std::endl;

    return 0;
}