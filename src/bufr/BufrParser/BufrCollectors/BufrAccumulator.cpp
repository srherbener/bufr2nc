/*
 * (C) Copyright 2020 NOAA/NWS/NCEP/EMC
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include "BufrAccumulator.h"


#include "eckit/exception/Exceptions.h"


namespace Ingester
{
    BufrAccumulator::BufrAccumulator(Eigen::Index numColumns, Eigen::Index blockSize) :
        dataArray_(blockSize, numColumns),
        numColumns_(numColumns),
        numDataRows_(0),
        blockSize_(blockSize)
    {
    }

    void BufrAccumulator::addRow(std::vector<FloatType>& newRow)
    {
        if (numDataRows_ + 1 > dataArray_.rows())
        {
            dataArray_.conservativeResize(dataArray_.rows() + blockSize_, numColumns_);
        }

        dataArray_.row(numDataRows_) = Eigen::Map<IngesterArray>(newRow.data(), 1, numColumns_);
        numDataRows_++;
    }

    IngesterArray BufrAccumulator::getData(Eigen::Index elementPos,
                                           Eigen::Index numElementsPerSet,
                                           const Channels& indices)
    {
        if (std::find_if(indices.begin(), indices.end(),
                         [](const auto x){ return x < 1; }) != indices.end())
        {
            throw eckit::BadParameter("All channel numbers must be >= 1.");
        }

        IngesterArray resultArr(numDataRows_, indices.size());
        unsigned int colIdx = 0;
        for (auto col : indices)
        {
            unsigned int offset = elementPos + numElementsPerSet * (col - 1);
            resultArr.block(0, colIdx, numDataRows_, 1) =
                dataArray_.block(0, offset, numDataRows_, 1);

            colIdx++;
        }

        return resultArr;
    }

    void BufrAccumulator::reset()
    {
        numDataRows_ = 0;
    }
}  // namespace Ingester
