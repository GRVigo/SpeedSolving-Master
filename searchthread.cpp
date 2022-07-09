#include "searchthread.h"

#include "lbl.h"
#include "cfop.h"
#include "roux.h"
#include "petrus.h"
#include "zz.h"
#include "ceor.h"
#include "mehta.h"
#include "nautilus.h"
#include "leor.h"

  /*****************************************************************************************************************/
 /*** LBL *********************************************************************************************************/
/*****************************************************************************************************************/

void RunSearch::startSearchLBL(const QString scr, const int CrossLayerIndex, const int NumCores, const int MetricIndex)
{
    // Select the layer for cross
    Lyr SearchLayer;
    switch (CrossLayerIndex)
    {
    case 0: case 1: case 4: SearchLayer = Lyr::D; break;
    case 2: case 6: SearchLayer = Lyr::B; break;
    case 3: case 8: SearchLayer = Lyr::L; break;
    case 5: SearchLayer = Lyr::U; break;
    case 7: SearchLayer = Lyr::F; break;
    case 9: SearchLayer = Lyr::R; break;
    default: SearchLayer = Lyr::U; break;
    }

    // Create search
    LBL* SearchLBL = new LBL(Algorithm(scr.toStdString()), NumCores);

    SearchLBL->SetMetric(static_cast<Metrics>(MetricIndex));

    // Start cross search
    emit msgLBLStartCross();
    if (!SearchLBL->SearchFLCross(SearchLayer))
    {
        delete SearchLBL;
        emit finishLBL_NoSolves();
        return;
    }

    // Complete first layer search
    emit msgLBLStartFL();
    SearchLBL->SearchFLCorners();

    // Start second layer search
    emit msgLBLStartSL();
    SearchLBL->SearchSLEdges();

    // Start last layer search
    emit msgLBLStartLL();
    SearchLBL->SearchLLCross1();
    SearchLBL->SearchLLCross2();
    SearchLBL->SearchLLCorners1();
    SearchLBL->SearchLLCorners2();

    emit finishLBL_Solves(QString::fromStdString(SearchLBL->GetTextScramble()),
                          QString("%1").arg(SearchLBL->GetCrossLayer()),
                          QString::fromStdString(SearchLBL->GetReport()));
    delete SearchLBL;
}

  /*****************************************************************************************************************/
 /*** CFOP ********************************************************************************************************/
/*****************************************************************************************************************/

void RunSearch::startSearchCFOP(const QString Scr, const int NumCores, const int BitConfig, const int VariantIndex, const int OptionIndex,
                                const int SpeedIndex, const int OrientIndex, const int AmountIndex, const int MetricIndex)
{
    const Algorithm Scramble(Scr.toStdString());

    bool CacheEnabled = BitConfig % 2 == 1;
    bool RegripsEnabled = (BitConfig >> 1) % 2 == 1;
    bool CancellationsEnabled = (BitConfig >> 2) % 2 == 1;

    // Read the allowed layers for cross
    std::vector<Lyr> CrossLayers;
    uint nLayers = GetCrossLayers(CrossLayers, OrientIndex);

    uint Depth;
    switch (SpeedIndex)
    {
    case 0: Depth = 6u; break;
    case 1: Depth = 7u; break;
    case 2: Depth = 8u; break;
    case 3: Depth = CacheEnabled ? 8u : 9u; break;
    default: Depth = 6u; break;
    }

    uint Inspections;
    switch (AmountIndex)
    {
    case 0: Inspections = 1u; break;
    case 1: if (nLayers > 5u) Inspections = 1u; else if (nLayers > 1u) Inspections = 3u; else Inspections = 6u; break;
    case 2: if (nLayers > 5u) Inspections = 2u; else if (nLayers > 1u) Inspections = 6u; else Inspections = 12u; break;
    case 3: if (nLayers > 5u) Inspections = 4u; else if (nLayers > 1u) Inspections = 12u; else Inspections = 24u; break;
    case 4: if (nLayers > 5u) Inspections = 8u; else if (nLayers > 1u) Inspections = 24u; else Inspections = 48u; break;
    default: Inspections = 1u; break;
    }

    // Create search
    CFOP* SearchCFOP = new CFOP(Algorithm(Scramble), NumCores);

    SearchCFOP->SetCrossLayers(CrossLayers);

    SearchCFOP->SetMetric(static_cast<Metrics>(MetricIndex));

    // Start cross search
    emit msgCFOPStartCross();

    if (CacheEnabled)
    {
        bool found = false;
        for (const auto& c : Cache) // Search in cache
        {
            if (c.Scramble == Scramble && c.Depth >= Depth && !c.Solves.empty())
            {
                emit msgCache();
                SearchCFOP->EvaluateCrosses(c.Solves, Inspections);
                SearchCFOP->SetTimeFS(c.Time);
                SearchCFOP->SetDepthFS(c.Depth);
                found = true;
                break;
            }
        }

        if (!found) // Solves not found in cache
        {
            Cache.clear(); // Cache memory usage can be large, clear previous cache
            const auto time_cross_start = std::chrono::system_clock::now();

            DeepSearch BaseSearch(Scramble);

            CurrentSearchMaxDepthPointer = BaseSearch.GetMaxDepthPointer();
            BaseSearch.SearchBase(Depth, Plc::BEST, NumCores);
            CurrentSearchMaxDepthPointer = nullptr;
            if (FinishingThread) { delete SearchCFOP; return; }

            if (BaseSearch.GetSolves().empty())
            {
                emit finishMethod_NoSolves();
                delete SearchCFOP;
                return;
            }

            SearchCFOP->EvaluateCrosses(BaseSearch.GetSolves(), Inspections);

            const std::chrono::duration<double> cross_elapsed_seconds = std::chrono::system_clock::now() - time_cross_start;
            SearchCFOP->SetTimeFS(cross_elapsed_seconds.count());
            SearchCFOP->SetDepthFS(Depth);

            // Add current solves into cache only in a full search
			if (!BaseSearch.CheckSkipSearch())
			{
				CacheUnit CU;
				CU.Scramble = Scramble;
				CU.Depth = Depth;
				CU.Time = SearchCFOP->GetTime();
				CU.Solves = BaseSearch.GetSolves();
				Cache.push_back(CU);
			}
        }
    }
    else // No cache
    {
        CurrentSearchMaxDepthPointer = SearchCFOP->GetMaxDepthPointer();
        if (!SearchCFOP->SearchCrosses(Depth, Inspections))
        {
			CurrentSearchMaxDepthPointer = nullptr;
            emit finishMethod_NoSolves();
            delete SearchCFOP;
            return;
        }
		CurrentSearchMaxDepthPointer = nullptr;
        if (FinishingThread) { delete SearchCFOP; return; }
    }

    // Start F2L search
    emit msgCFOPStartF2L();
    CurrentSearchMaxDepthPointer = SearchCFOP->GetMaxDepthPointer();
    SearchCFOP->SearchF2L();
	CurrentSearchMaxDepthPointer = nullptr;
    if (FinishingThread) { delete SearchCFOP; return; }
	
    // Start last layer search
    emit msgCFOPStartLL();
    if (VariantIndex == 1)
    {
        SearchCFOP->Search1LLL();
    }
    else if (VariantIndex == 2)
    {
        CurrentSearchMaxDepthPointer = SearchCFOP->GetMaxDepthPointer();
        SearchCFOP->SearchEOLL();
		CurrentSearchMaxDepthPointer = nullptr;
        if (FinishingThread) { delete SearchCFOP; return; }
        SearchCFOP->SearchZBLL();
    }
    else // Default
    {
        SearchCFOP->SearchOLL();
        SearchCFOP->SearchPLL();
    }

    if (RegripsEnabled) SearchCFOP->SetRegrips();

    emit finishMethod_Solves(QString::fromStdString(SearchCFOP->GetTextScramble()),
                             QString::fromStdString(SearchCFOP->GetReport(CancellationsEnabled, false)));
    emit finishMethod_Best(QString::fromStdString(SearchCFOP->GetBestReport(CancellationsEnabled)));
    emit finishMethod_Time(QString::fromStdString(SearchCFOP->GetTimeReport()));

    delete SearchCFOP;
}

  /*****************************************************************************************************************/
 /*** Roux ********************************************************************************************************/
/*****************************************************************************************************************/

void RunSearch::startSearchRoux(const QString Scr, const int NumCores, const int BitConfig, const int VariantIndex, const int OptionIndex,
                                const int SpeedIndex, const int OrientIndex, const int AmountIndex, const int MetricIndex)
{
    const Algorithm Scramble(Scr.toStdString());

    const bool CacheEnabled = BitConfig % 2 == 1;
    const bool RegripsEnabled = (BitConfig >> 1) % 2 == 1;
    const bool CancellationsEnabled = (BitConfig >> 2) % 2 == 1;

    // Read the allowed orientations
    std::vector<Spn> SearchSpins;
    uint nSpins = GetSearchSpins(SearchSpins, OrientIndex);

    uint Depth1, Depth2;
    switch (SpeedIndex)
    {
    case 0: Depth1 = 6u; Depth2 = 6u; break;
    case 1: Depth1 = 7u; Depth2 = 6u; break;
    case 2: Depth1 = 8u; Depth2 = 7u; break;
    case 3: Depth1 = 8u; Depth2 = 8u; break;
    default: Depth1 = 6u; Depth2 = 6u; break;
    }

    uint Inspections;
    switch (AmountIndex)
    {
    case 0: Inspections = 1u; break;
    case 1: if (nSpins > 20u) Inspections = 1u; else if (nSpins > 4u) Inspections = 3u; else Inspections = 6u; break;
    case 2: if (nSpins > 20u) Inspections = 2u; else if (nSpins > 4u) Inspections = 6u; else Inspections = 12u; break;
    case 3: if (nSpins > 20u) Inspections = 4u; else if (nSpins > 4u) Inspections = 12u; else Inspections = 24u; break;
    case 4: if (nSpins > 20u) Inspections = 8u; else if (nSpins > 4u) Inspections = 24u; else Inspections = 48u; break;
    default: Inspections = 1u; break;
    }

    // Create search
    Roux* SearchRoux = new Roux(Scramble, NumCores);

    SearchRoux->SetSearchSpins(SearchSpins);

    SearchRoux->SetMetric(static_cast<Metrics>(MetricIndex));

    // Start first block search
    emit msgRouxStartFB();

    if (CacheEnabled)
    {
        bool found = false;
        for (const auto& c : Cache) // Search in cache
        {
            if (c.Scramble == Scramble && c.Depth >= Depth1 && !c.Solves.empty())
            {
                emit msgCache();
                SearchRoux->EvaluateFirstBlock(c.Solves, Inspections);
                SearchRoux->SetTimeFS(c.Time);
                SearchRoux->SetDepthFS(c.Depth);
                found = true;
                break;
            }
        }

        if (!found) // Solves not found in cache
        {
            Cache.clear(); // Cache memory usage can be large, clear previous cache
            const auto time_fb_start = std::chrono::system_clock::now();

            DeepSearch BaseSearch(Scramble);
            CurrentSearchMaxDepthPointer = BaseSearch.GetMaxDepthPointer();
            BaseSearch.SearchBase(Depth1, Plc::BEST, NumCores);
            CurrentSearchMaxDepthPointer = nullptr;
            if (FinishingThread) { delete SearchRoux; return; }

            if (BaseSearch.GetSolves().empty())
            {
                emit finishMethod_NoSolves();
				delete SearchRoux;
                return;
            }

            SearchRoux->EvaluateFirstBlock(BaseSearch.GetSolves(), Inspections);

            const std::chrono::duration<double> fb_elapsed_seconds = std::chrono::system_clock::now() - time_fb_start;
            SearchRoux->SetTimeFS(fb_elapsed_seconds.count());
            SearchRoux->SetDepthFS(Depth1);

            // Add current solves into cache only in a full search
            if (!BaseSearch.CheckSkipSearch())
            {
                CacheUnit CU;
                CU.Scramble = Scramble;
                CU.Depth = Depth1;
                CU.Time = SearchRoux->GetTimeFB();
                CU.Solves = BaseSearch.GetSolves();
                Cache.push_back(CU);
            }
        }
    }
    else // No cache
    {
        CurrentSearchMaxDepthPointer = SearchRoux->GetMaxDepthPointer();
        if (!SearchRoux->SearchFirstBlock(Depth1, Inspections))
        {
            CurrentSearchMaxDepthPointer = nullptr;
            emit finishMethod_NoSolves();
            delete SearchRoux;
            return;
        }
        CurrentSearchMaxDepthPointer = nullptr;
        if (FinishingThread) { delete SearchRoux; return; }
    }

    // Start second blocks search
    emit msgRouxStartSB();
    CurrentSearchMaxDepthPointer = SearchRoux->GetMaxDepthPointer();
    SearchRoux->SearchSecondBlocksFirstSquare(Depth2);
    SearchRoux->SearchSecondBlocksSecondSquare(4u);
    CurrentSearchMaxDepthPointer = nullptr;
    if (FinishingThread) { delete SearchRoux; return; }

    if (VariantIndex == 1)
    {
        // Start COLL search
        emit msgRouxStartCOLL();
        SearchRoux->SearchCOLL();
    }
    else // if (VariantIndex == 0)
    {
        // Start CMLL search
        emit msgRouxStartCMLL();
        SearchRoux->SearchCMLL();
    }

    // Start L6E search
    emit msgRouxStartL6E();
    if (OptionIndex == 1)
    {
        CurrentSearchMaxDepthPointer = SearchRoux->GetMaxDepthPointer();
        SearchRoux->SearchL6E(16u);
        CurrentSearchMaxDepthPointer = nullptr;
        if (FinishingThread) { delete SearchRoux; return; }
    }
    else // if (OptionIndex == 0)
    {
        CurrentSearchMaxDepthPointer = SearchRoux->GetMaxDepthPointer();
        SearchRoux->SearchL6EO(10u);
        SearchRoux->SearchL6E2E(12u);
        SearchRoux->SearchL6E(15u);
        CurrentSearchMaxDepthPointer = nullptr;
        if (FinishingThread) { delete SearchRoux; return; }
    }

    if (RegripsEnabled) SearchRoux->SetRegrips();

    emit finishMethod_Solves(QString::fromStdString(SearchRoux->GetTextScramble()),
                             QString::fromStdString(SearchRoux->GetReport(CancellationsEnabled, false)));
    emit finishMethod_Best(QString::fromStdString(SearchRoux->GetBestReport(CancellationsEnabled)));
    emit finishMethod_Time(QString::fromStdString(SearchRoux->GetTimeReport()));

    delete SearchRoux;
}

  /*****************************************************************************************************************/
 /*** Petrus ******************************************************************************************************/
/*****************************************************************************************************************/

void RunSearch::startSearchPetrus(const QString Scr, const int NumCores, const int BitConfig, const int VariantIndex, const int OptionIndex,
                                  const int SpeedIndex, const int OrientIndex, const int AmountIndex, const int MetricIndex)
{
    const Algorithm Scramble(Scr.toStdString());

    const bool CacheEnabled = BitConfig % 2 == 1;
    const bool RegripsEnabled = (BitConfig >> 1) % 2 == 1;
    const bool CancellationsEnabled = (BitConfig >> 2) % 2 == 1;

    // Read the allowed orientations
    std::vector<Spn> SearchSpins;
    uint nSpins = GetSearchSpins(SearchSpins, OrientIndex);

    uint Depth1, Depth2;
    switch (SpeedIndex)
    {
    case 0: Depth1 = 6u; Depth2 = 6u; break;
    case 1: Depth1 = 6u; Depth2 = 7u; break;
    case 2: Depth1 = 7u; Depth2 = 7u; break;
    case 3: Depth1 = 8u; Depth2 = 8u; break;
    default: Depth1 = 6u; Depth2 = 6u; break;
    }

    uint Inspections;
    switch (AmountIndex)
    {
    case 0: Inspections = 1u; break;
    case 1: if (nSpins > 20u) Inspections = 1u; else if (nSpins > 4u) Inspections = 3u; else Inspections = 6u; break;
    case 2: if (nSpins > 20u) Inspections = 2u; else if (nSpins > 4u) Inspections = 6u; else Inspections = 12u; break;
    case 3: if (nSpins > 20u) Inspections = 4u; else if (nSpins > 4u) Inspections = 12u; else Inspections = 24u; break;
    case 4: if (nSpins > 20u) Inspections = 8u; else if (nSpins > 4u) Inspections = 24u; else Inspections = 48u; break;
    default: Inspections = 1u; break;
    }

    // Create search
    Petrus* SearchPetrus = new Petrus(Scramble, NumCores);

    SearchPetrus->SetSearchSpins(SearchSpins);

    SearchPetrus->SetMetric(static_cast<Metrics>(MetricIndex));

    // Start blocks search
    emit msgPetrusStartFB();

    if (CacheEnabled)
    {
        bool found = false;
        for (const auto& c : Cache) // Search in cache
        {
            if (c.Scramble == Scramble && c.Depth >= Depth1 && !c.Solves.empty())
            {
                emit msgCache();
                SearchPetrus->EvaluateBlock(c.Solves, Inspections);
                SearchPetrus->SetTimeFS(c.Time);
                SearchPetrus->SetDepthFS(c.Depth);
                found = true;
                break;
            }
        }

        if (!found) // Solves not found in cache
        {
            Cache.clear(); // Cache memory usage can be large, clear previous cache
            const auto time_block_start = std::chrono::system_clock::now();

            DeepSearch BaseSearch(Scramble);
            CurrentSearchMaxDepthPointer = BaseSearch.GetMaxDepthPointer();
            BaseSearch.SearchBase(Depth1, Plc::BEST, NumCores);
            CurrentSearchMaxDepthPointer = nullptr;
            if (FinishingThread) { delete SearchPetrus; return; }

            if (BaseSearch.GetSolves().empty())
            {
                emit finishMethod_NoSolves();
                delete SearchPetrus;
                return;
            }

            SearchPetrus->EvaluateBlock(BaseSearch.GetSolves(), Inspections);

            const std::chrono::duration<double> block_elapsed_seconds = std::chrono::system_clock::now() - time_block_start;
            SearchPetrus->SetTimeFS(block_elapsed_seconds.count());
            SearchPetrus->SetDepthFS(Depth1);

            // Add current solves into cache only in a full search
            if (!BaseSearch.CheckSkipSearch())
            {
                CacheUnit CU;
                CU.Scramble = Scramble;
                CU.Depth = Depth1;
                CU.Time = SearchPetrus->GetTimeBlock();
                CU.Solves = BaseSearch.GetSolves();
                Cache.push_back(CU);
            }
        }
    }
    else // No cache
    {
        CurrentSearchMaxDepthPointer = SearchPetrus->GetMaxDepthPointer();
        if (!SearchPetrus->SearchBlock(Depth1, Inspections))
        {
            CurrentSearchMaxDepthPointer = nullptr;
            emit finishMethod_NoSolves();
            delete SearchPetrus;
            return;
        }
        CurrentSearchMaxDepthPointer = nullptr;
        if (FinishingThread) { delete SearchPetrus; return; }
    }

    // Start expanded blocks search
    emit msgPetrusStartEB();
    CurrentSearchMaxDepthPointer = SearchPetrus->GetMaxDepthPointer();
    SearchPetrus->SearchExpandedBlock();
    CurrentSearchMaxDepthPointer = nullptr;
    if (FinishingThread) { delete SearchPetrus; return; }

    // Start EO search
    emit msgPetrusStartEO();
    CurrentSearchMaxDepthPointer = SearchPetrus->GetMaxDepthPointer();
    SearchPetrus->SearchEO();
    CurrentSearchMaxDepthPointer = nullptr;
    if (FinishingThread) { delete SearchPetrus; return; }

    // Start F2L search
    emit msgPetrusStartF2L();
    CurrentSearchMaxDepthPointer = SearchPetrus->GetMaxDepthPointer();
    SearchPetrus->SearchF2L(Depth2);
    CurrentSearchMaxDepthPointer = nullptr;
    if (FinishingThread) { delete SearchPetrus; return; }

    // Start last layer search
    emit msgPetrusStartLL();
    if (VariantIndex == 1)
    {
        SearchPetrus->SearchOCLL(); // Start OCLL search
        SearchPetrus->SearchPLL(); // Start PLL search
    }
    else if (VariantIndex == 2)
    {
        SearchPetrus->SearchCOLL(); // Start COLL search
        SearchPetrus->SearchEPLL(); // Start EPLL search
    }
    else // if (VariantIndex == 0)
    {
        SearchPetrus->SearchZBLL(); // Start ZBLL search
    }

    if (RegripsEnabled) SearchPetrus->SetRegrips();

    emit finishMethod_Solves(QString::fromStdString(SearchPetrus->GetTextScramble()),
                             QString::fromStdString(SearchPetrus->GetReport(CancellationsEnabled, false)));
    emit finishMethod_Best(QString::fromStdString(SearchPetrus->GetBestReport(CancellationsEnabled)));
    emit finishMethod_Time(QString::fromStdString(SearchPetrus->GetTimeReport()));

    delete SearchPetrus;
}

  /*****************************************************************************************************************/
 /*** ZZ **********************************************************************************************************/
/*****************************************************************************************************************/

void RunSearch::startSearchZZ(const QString Scr, const int NumCores, const int BitConfig, const int VariantIndex, const int OptionIndex,
                              const int SpeedIndex, const int OrientIndex, const int AmountIndex, const int MetricIndex)
{
    const Algorithm Scramble(Scr.toStdString());

    const bool CacheEnabled = BitConfig % 2 == 1;
    const bool RegripsEnabled = (BitConfig >> 1) % 2 == 1;
    const bool CancellationsEnabled = (BitConfig >> 2) % 2 == 1;

    // Read the allowed orientations
    std::vector<Spn> SearchSpins;
    uint nSpins = GetSearchSpins(SearchSpins, OrientIndex);

    uint Depth;
    switch (SpeedIndex)
    {
    case 0: Depth = 6u; break;
    case 1: Depth = 7u; break;
    case 2: Depth = 8u; break;
    case 3: Depth = 8u; break;
    default: Depth = 6u; break;
    }

    uint Inspections;
    switch (AmountIndex)
    {
    case 0: Inspections = 1u; break;
    case 1: if (nSpins > 20u) Inspections = 1u; else if (nSpins > 4u) Inspections = 3u; else Inspections = 6u; break;
    case 2: if (nSpins > 20u) Inspections = 2u; else if (nSpins > 4u) Inspections = 6u; else Inspections = 12u; break;
    case 3: if (nSpins > 20u) Inspections = 4u; else if (nSpins > 4u) Inspections = 12u; else Inspections = 24u; break;
    case 4: if (nSpins > 20u) Inspections = 8u; else if (nSpins > 4u) Inspections = 24u; else Inspections = 48u; break;
    default: Inspections = 1u; break;
    }

    // Create search
    ZZ* SearchZZ = new ZZ(Scramble, NumCores);

    SearchZZ->SetSearchSpins(SearchSpins);

    SearchZZ->SetMetric(static_cast<Metrics>(MetricIndex));

    // Start EOX search
    emit msgZZStartEOX();

    if (CacheEnabled)
    {
        bool found = false;
        for (const auto& c : Cache) // Search in cache
        {
          if (c.Scramble == Scramble && c.Depth >= Depth && !c.Solves.empty())
          {
              emit msgCache();
              SearchZZ->EvaluateEOX(c.Solves, Inspections);
              SearchZZ->SetTimeFS(c.Time);
              SearchZZ->SetDepthFS(c.Depth);
              found = true;
              break;
          }
        }

        if (!found) // Solves not found in cache
        {
            Cache.clear(); // Cache memory usage can be large, clear previous cache
            const auto time_eoline_start = std::chrono::system_clock::now();

            DeepSearch BaseSearch(Scramble);
            CurrentSearchMaxDepthPointer = BaseSearch.GetMaxDepthPointer();
            BaseSearch.SearchBase(Depth, Plc::BEST, NumCores);
            CurrentSearchMaxDepthPointer = nullptr;
            if (FinishingThread) { delete SearchZZ; return; }

            if (BaseSearch.GetSolves().empty())
            {
                emit finishMethod_NoSolves();
                delete SearchZZ;
                return;
            }

            SearchZZ->EvaluateEOX(BaseSearch.GetSolves(), Inspections);

            const std::chrono::duration<double> eoline_elapsed_seconds = std::chrono::system_clock::now() - time_eoline_start;
            SearchZZ->SetTimeFS(eoline_elapsed_seconds.count());
            SearchZZ->SetDepthFS(Depth);

            // Add current solves into cache only in a full search
            if (!BaseSearch.CheckSkipSearch())
            {
                CacheUnit CU;
                CU.Scramble = Scramble;
                CU.Depth = Depth;
                CU.Time = SearchZZ->GetTimeEOX();
                CU.Solves = BaseSearch.GetSolves();
                Cache.push_back(CU);
            }
        }
    }
    else // No cache
    {
        CurrentSearchMaxDepthPointer = SearchZZ->GetMaxDepthPointer();
        if (!SearchZZ->SearchEOX(Depth, Inspections))
        {
            CurrentSearchMaxDepthPointer = nullptr;
            emit finishMethod_NoSolves();
            delete SearchZZ;
            return;
        }
        CurrentSearchMaxDepthPointer = nullptr;
        if (FinishingThread) { delete SearchZZ; return; }
    }

    // Start F2L search
    emit msgZZStartF2L();
    CurrentSearchMaxDepthPointer = SearchZZ->GetMaxDepthPointer();
    SearchZZ->SearchF2L();
    CurrentSearchMaxDepthPointer = nullptr;
    if (FinishingThread) { delete SearchZZ; return; }

    // Start last layer search
    emit msgZZStartLL();
    if (VariantIndex== 1)
    {
      SearchZZ->SearchOCLL(); // Start OCLL search
      SearchZZ->SearchPLL(); // Start PLL search
    }
    else if (VariantIndex == 2)
    {
      SearchZZ->SearchCOLL(); // Start COLL search
      SearchZZ->SearchEPLL(); // Start EPLL search
    }
    else // if (VariantIndex == 0)
    {
      SearchZZ->SearchZBLL(); // Start ZBLL search
    }

    if (RegripsEnabled) SearchZZ->SetRegrips();

    emit finishMethod_Solves(QString::fromStdString(SearchZZ->GetTextScramble()),
                             QString::fromStdString(SearchZZ->GetReport(CancellationsEnabled, false)));
    emit finishMethod_Best(QString::fromStdString(SearchZZ->GetBestReport(CancellationsEnabled)));
    emit finishMethod_Time(QString::fromStdString(SearchZZ->GetTimeReport()));

    delete SearchZZ;
}

  /*****************************************************************************************************************/
 /*** CEOR ********************************************************************************************************/
/*****************************************************************************************************************/

void RunSearch::startSearchCEOR(const QString Scr, const int NumCores, const int BitConfig, const int VariantIndex, const int OptionIndex,
                                const int SpeedIndex, const int OrientIndex, const int AmountIndex, const int MetricIndex)
{
    const Algorithm Scramble(Scr.toStdString());

    const bool CacheEnabled = BitConfig % 2 == 1;
    const bool RegripsEnabled = (BitConfig >> 1) % 2 == 1;
    const bool CancellationsEnabled = (BitConfig >> 2) % 2 == 1;

    // Read the allowed orientations
    std::vector<Spn> SearchSpins;
    uint nSpins = GetSearchSpins(SearchSpins, OrientIndex);

    uint Depth1, Depth2;
    switch (SpeedIndex)
    {
    case 0: Depth1 = 6u; Depth2 = 5u; break;
    case 1: Depth1 = 6u; Depth2 = 6u; break;
    case 2: Depth1 = 7u; Depth2 = 6u; break;
    case 3: Depth1 = 7u; Depth2 = 7u; break;
    default: Depth1 = 6u; Depth2 = 5u; break;
    }

    uint Inspections;
    switch (AmountIndex)
    {
    case 0: Inspections = 1u; break;
    case 1: if (nSpins > 20u) Inspections = 1u; else if (nSpins > 4u) Inspections = 3u; else Inspections = 6u; break;
    case 2: if (nSpins > 20u) Inspections = 2u; else if (nSpins > 4u) Inspections = 6u; else Inspections = 12u; break;
    case 3: if (nSpins > 20u) Inspections = 4u; else if (nSpins > 4u) Inspections = 12u; else Inspections = 24u; break;
    case 4: if (nSpins > 20u) Inspections = 8u; else if (nSpins > 4u) Inspections = 24u; else Inspections = 48u; break;
    default: Inspections = 1u; break;
    }

    // Create search
    CEOR* SearchYruRU = new CEOR(Scramble, NumCores);

    SearchYruRU->SetSearchSpins(SearchSpins);

    SearchYruRU->SetMetric(static_cast<Metrics>(MetricIndex));

    if (OptionIndex == 1)
    {
        // Start Line search
        emit msgCEORStartLines();
        if (CacheEnabled)
        {
            bool found = false;
            for (const auto& c : Cache) // Search in cache
            {
              if (c.Scramble == Scramble && c.Depth >= 6u && !c.Solves.empty())
              {
                  emit msgCache();
                  SearchYruRU->EvaluateLines(c.Solves, Inspections);
                  SearchYruRU->SetTimeFS(c.Time);
                  SearchYruRU->SetDepthFS(c.Depth);
                  found = true;
                  break;
              }
            }

            if (!found) // Solves not found in cache
            {
                Cache.clear(); // Cache memory usage can be large, clear previous cache
                const auto time_line_start = std::chrono::system_clock::now();

                DeepSearch BaseSearch(Scramble);
                CurrentSearchMaxDepthPointer = BaseSearch.GetMaxDepthPointer();
                BaseSearch.SearchBase(6u, Plc::BEST, NumCores);
                CurrentSearchMaxDepthPointer = nullptr;
                if (FinishingThread) { delete SearchYruRU; return; }

                if (BaseSearch.GetSolves().empty())
                {
                    emit finishMethod_NoSolves();
                    delete SearchYruRU;
                    return;
                }

                SearchYruRU->EvaluateLines(BaseSearch.GetSolves(), Inspections);

                const std::chrono::duration<double> line_elapsed_seconds = std::chrono::system_clock::now() - time_line_start;
                SearchYruRU->SetTimeFS(line_elapsed_seconds.count());
                SearchYruRU->SetDepthFS(6u);

                // Add current solves into cache only in a full search
                if (!BaseSearch.CheckSkipSearch())
                {
                    CacheUnit CU;
                    CU.Scramble = Scramble;
                    CU.Depth = 6u;
                    CU.Time = SearchYruRU->GetTimeLines();
                    CU.Solves = BaseSearch.GetSolves();
                    Cache.push_back(CU);
                }
            }
        }
        else // No cache
        {
            CurrentSearchMaxDepthPointer = SearchYruRU->GetMaxDepthPointer();
            if (!SearchYruRU->SearchLines(6u, Inspections))
            {
                CurrentSearchMaxDepthPointer = nullptr;
                emit finishMethod_NoSolves();
                delete SearchYruRU;
                return;
            }
            CurrentSearchMaxDepthPointer = nullptr;
            if (FinishingThread) { delete SearchYruRU; return; }
        }

        // Start CP search
        emit msgCEORStartCP();
        CurrentSearchMaxDepthPointer = SearchYruRU->GetMaxDepthPointer();
        SearchYruRU->SearchCP();
        CurrentSearchMaxDepthPointer = nullptr;
        if (FinishingThread) { delete SearchYruRU; return; }
    }
    else // if (OptionIndex == 0)
    {
        // Start Line+CP search
        emit msgCEORStartCPLines();

        if (CacheEnabled)
        {
            bool found = false;
            for (const auto& c : Cache) // Search in cache
            {
                if (c.Scramble == Scramble && c.Depth >= 6u && !c.Solves.empty())
                {
                    emit msgCache();
                    SearchYruRU->EvaluateCPLines(c.Solves, Inspections);
                    SearchYruRU->SetTimeFS(c.Time);
                    SearchYruRU->SetDepthFS(c.Depth);
                    found = true;
                    break;
                }
            }

            if (!found) // Solves not found in cache
            {
                Cache.clear(); // Cache memory usage can be large, clear previous cache
                const auto time_cpline_start = std::chrono::system_clock::now();

                DeepSearch BaseSearch(Scramble);
                BaseSearch.SearchBase(6u, Plc::BEST, NumCores);

                if (BaseSearch.GetSolves().empty())
                {
                    emit finishMethod_NoSolves();
                    delete SearchYruRU;
                  return;
                }

                SearchYruRU->EvaluateCPLines(BaseSearch.GetSolves(), Inspections);

                const std::chrono::duration<double> cpline_elapsed_seconds = std::chrono::system_clock::now() - time_cpline_start;
                SearchYruRU->SetTimeFS(cpline_elapsed_seconds.count());
                SearchYruRU->SetDepthFS(6u);

                // Add current solves into cache only in a full search
                if (!BaseSearch.CheckSkipSearch())
                {
                    CacheUnit CU;
                    CU.Scramble = Scramble;
                    CU.Depth = 6u;
                    CU.Time = SearchYruRU->GetTimeCPLines();
                    CU.Solves = BaseSearch.GetSolves();
                    Cache.push_back(CU);
                }
            }
        }
        else // No cache
        {
            CurrentSearchMaxDepthPointer = SearchYruRU->GetMaxDepthPointer();
            if (!SearchYruRU->SearchCPLines(6u, Inspections))
            {
                CurrentSearchMaxDepthPointer = nullptr;
                emit finishMethod_NoSolves();
                delete SearchYruRU;
                return;
            }
            CurrentSearchMaxDepthPointer = nullptr;
            if (FinishingThread) { delete SearchYruRU; return; }
        }
    }

    // Start pEO-extension search
    emit msgCEORStartpEO();
    CurrentSearchMaxDepthPointer = SearchYruRU->GetMaxDepthPointer();
    SearchYruRU->SearchpEO(Depth1);
    CurrentSearchMaxDepthPointer = nullptr;
    if (FinishingThread) { delete SearchYruRU; return; }

    // Start EOBF search
    emit msgCEORStartEOBF();
    CurrentSearchMaxDepthPointer = SearchYruRU->GetMaxDepthPointer();
    SearchYruRU->SearchEO(9u);
    SearchYruRU->SearchEOBF(Depth2);
    CurrentSearchMaxDepthPointer = nullptr;
    if (FinishingThread) { delete SearchYruRU; return; }

    // Start F2L search
    emit msgCEORStartF2L();
    CurrentSearchMaxDepthPointer = SearchYruRU->GetMaxDepthPointer();
    SearchYruRU->SearchF2L(13u);
    CurrentSearchMaxDepthPointer = nullptr;
    if (FinishingThread) { delete SearchYruRU; return; }

    // Start last layer search
    emit msgCEORStartLL();
    SearchYruRU->Search2GLL();

    if (RegripsEnabled) SearchYruRU->SetRegrips();

    emit finishMethod_Solves(QString::fromStdString(SearchYruRU->GetTextScramble()),
                             QString::fromStdString(SearchYruRU->GetReport(CancellationsEnabled, false)));
    emit finishMethod_Best(QString::fromStdString(SearchYruRU->GetBestReport(CancellationsEnabled)));
    emit finishMethod_Time(QString::fromStdString(SearchYruRU->GetTimeReport()));

    delete SearchYruRU;
}

  /*****************************************************************************************************************/
 /*** Mehta *******************************************************************************************************/
/*****************************************************************************************************************/

void RunSearch::startSearchMehta(const QString Scr, const int NumCores, const int BitConfig, const int VariantIndex, const int OptionIndex,
                                const int SpeedIndex, const int OrientIndex, const int AmountIndex, const int MetricIndex)
{
    const Algorithm Scramble(Scr.toStdString());

    const bool CacheEnabled = BitConfig % 2 == 1;
    const bool RegripsEnabled = (BitConfig >> 1) % 2 == 1;
    const bool CancellationsEnabled = (BitConfig >> 2) % 2 == 1;

    // Read the allowed orientations
    std::vector<Spn> SearchSpins;
    uint nSpins = GetSearchSpins(SearchSpins, OrientIndex);

    uint Depth1, Depth2;
    switch (SpeedIndex)
    {
    case 0: Depth1 = 6u; Depth2 = 6u; break;
    case 1: Depth1 = 7u; Depth2 = 6u; break;
    case 2: Depth1 = 7u; Depth2 = 7u; break;
    case 3: Depth1 = 8u; Depth2 = 7u; break;
    default: Depth1 = 6u; Depth2 = 6u; break;
    }

    uint Inspections;
    switch (AmountIndex)
    {
    case 0: Inspections = 1u; break;
    case 1: if (nSpins > 20u) Inspections = 1u; else if (nSpins > 4u) Inspections = 3u; else Inspections = 6u; break;
    case 2: if (nSpins > 20u) Inspections = 2u; else if (nSpins > 4u) Inspections = 6u; else Inspections = 12u; break;
    case 3: if (nSpins > 20u) Inspections = 4u; else if (nSpins > 4u) Inspections = 12u; else Inspections = 24u; break;
    case 4: if (nSpins > 20u) Inspections = 8u; else if (nSpins > 4u) Inspections = 24u; else Inspections = 48u; break;
    default: Inspections = 1u; break;
    }

    // Create search
    Mehta* SearchMehta = new Mehta(Scramble, NumCores);

    SearchMehta->SetSearchSpins(SearchSpins);

    SearchMehta->SetMetric(static_cast<Metrics>(MetricIndex));

    // Start first blocks search
    emit msgMehtaStartFB();

    if (CacheEnabled)
    {
        bool found = false;
        for (const auto& c : Cache) // Search in cache
        {
            if (c.Scramble == Scramble && c.Depth >= Depth1 && !c.Solves.empty())
            {
                emit msgCache();
                SearchMehta->EvaluateFB(c.Solves, Inspections);
                SearchMehta->SetTimeFS(c.Time);
                SearchMehta->SetDepthFS(c.Depth);
                found = true;
                break;
            }
        }

        if (!found) // Solves not found in cache
        {
            Cache.clear(); // Cache memory usage can be large, clear previous cache
            const auto time_fb_start = std::chrono::system_clock::now();

            DeepSearch BaseSearch(Scramble);
            BaseSearch.SearchBase(Depth1, Plc::BEST, NumCores);

            if (BaseSearch.GetSolves().empty())
            {
                emit finishMethod_NoSolves();
                delete SearchMehta;
                return;
            }

            SearchMehta->EvaluateFB(BaseSearch.GetSolves(), Inspections);

            const std::chrono::duration<double> fb_elapsed_seconds = std::chrono::system_clock::now() - time_fb_start;
            SearchMehta->SetTimeFS(fb_elapsed_seconds.count());
            SearchMehta->SetDepthFS(Depth1);

            // Add current solves into cache only in a full search
            if (!BaseSearch.CheckSkipSearch())
            {
                CacheUnit CU;
                CU.Scramble = Scramble;
                CU.Depth = Depth1;
                CU.Time = SearchMehta->GetTimeFB();
                CU.Solves = BaseSearch.GetSolves();
                Cache.push_back(CU);
            }
        }
    }
    else // No cache
    {
        CurrentSearchMaxDepthPointer = SearchMehta->GetMaxDepthPointer();
        if (!SearchMehta->SearchFB(Depth1, Inspections))
        {
            CurrentSearchMaxDepthPointer = nullptr;
            emit finishMethod_NoSolves();
            delete SearchMehta;
            return;
        }
        CurrentSearchMaxDepthPointer = nullptr;
        if (FinishingThread) { delete SearchMehta; return; }
    }

    // Start 3QB search
    emit msgMehtaStart3QB();
    CurrentSearchMaxDepthPointer = SearchMehta->GetMaxDepthPointer();
    SearchMehta->Search3QB(Depth2);
    CurrentSearchMaxDepthPointer = nullptr;
    if (FinishingThread) { delete SearchMehta; return; }

    // Start EOLE search
    emit msgMehtaStartEOLE();
    SearchMehta->SearchEOLE();

    if (VariantIndex == 1)
    {
        emit msgMehtaStart6CO();
        SearchMehta->Search6CO();

        emit msgMehtaStart6CP();
        SearchMehta->Search6CP();

        emit msgMehtaStartL5EP();
        SearchMehta->SearchL5EP();
    }
    else if (VariantIndex == 2)
    {
        emit msgMehtaStart6CO();
        SearchMehta->Search6CO();

        emit msgMehtaStartAPDR();
        SearchMehta->SearchAPDR();

        emit msgMehtaStartPLL();
        SearchMehta->SearchPLL();
    }
    else if (VariantIndex == 3)
    {
        emit msgMehtaStartDCAL();
        SearchMehta->SearchDCAL();

        emit msgMehtaStartCDRLL();
        SearchMehta->SearchCDRLL();

        emit msgMehtaStartL5EP();
        SearchMehta->SearchL5EP();
    }
    else if (VariantIndex == 4)
    {
        emit msgMehtaStartDCAL();
        SearchMehta->SearchDCAL();

        emit msgMehtaStartJTLE();
        SearchMehta->SearchJTLE();

        emit msgMehtaStartPLL();
        SearchMehta->SearchPLL();
    }
    else // Mehta-TDR
    {
        emit msgMehtaStartTDR();
        SearchMehta->SearchTDR();

        emit msgMehtaStartZBLL();
        SearchMehta->SearchZBLL();
    }

    if (RegripsEnabled) SearchMehta->SetRegrips();

    emit finishMethod_Solves(QString::fromStdString(SearchMehta->GetTextScramble()),
                             QString::fromStdString(SearchMehta->GetReport(CancellationsEnabled, false)));
    emit finishMethod_Best(QString::fromStdString(SearchMehta->GetBestReport(CancellationsEnabled)));
    emit finishMethod_Time(QString::fromStdString(SearchMehta->GetTimeReport()));

    delete SearchMehta;
}

  /*****************************************************************************************************************/
 /*** Nautilus ****************************************************************************************************/
/*****************************************************************************************************************/

void RunSearch::startSearchNautilus(const QString Scr, const int NumCores, const int BitConfig, const int VariantIndex, const int OptionIndex,
                                    const int SpeedIndex, const int OrientIndex, const int AmountIndex, const int MetricIndex)
{
    const Algorithm Scramble(Scr.toStdString());

    const bool CacheEnabled = BitConfig % 2 == 1;
    const bool RegripsEnabled = (BitConfig >> 1) % 2 == 1;
    const bool CancellationsEnabled = (BitConfig >> 2) % 2 == 1;

    // Read the allowed orientations
    std::vector<Spn> SearchSpins;
    uint nSpins = GetSearchSpins(SearchSpins, OrientIndex);

    uint Depth1, Depth2;
    switch (SpeedIndex)
    {
    case 0: Depth1 = 6u; Depth2 = 6u; break;
    case 1: Depth1 = 7u; Depth2 = 6u; break;
    case 2: Depth1 = 7u; Depth2 = 7u; break;
    case 3: Depth1 = 8u; Depth2 = 7u; break;
    default: Depth1 = 6u; Depth2 = 6u; break;
    }

    uint Inspections;
    switch (AmountIndex)
    {
    case 0: Inspections = 1u; break;
    case 1: if (nSpins > 20u) Inspections = 1u; else if (nSpins > 4u) Inspections = 3u; else Inspections = 6u; break;
    case 2: if (nSpins > 20u) Inspections = 2u; else if (nSpins > 4u) Inspections = 6u; else Inspections = 12u; break;
    case 3: if (nSpins > 20u) Inspections = 4u; else if (nSpins > 4u) Inspections = 12u; else Inspections = 24u; break;
    case 4: if (nSpins > 20u) Inspections = 8u; else if (nSpins > 4u) Inspections = 24u; else Inspections = 48u; break;
    default: Inspections = 1u; break;
    }

    // Create search
    Nautilus* SearchNautilus = new Nautilus(Scramble, NumCores);

    SearchNautilus->SetSearchSpins(SearchSpins);

    SearchNautilus->SetMetric(static_cast<Metrics>(MetricIndex));

    // Start first blocks search
    emit msgNautilusStartFB();

    if (CacheEnabled)
    {
        bool found = false;
        for (const auto& c : Cache) // Search in cache
        {
            if (c.Scramble == Scramble && c.Depth >= Depth1 && !c.Solves.empty())
            {
                emit msgCache();
                SearchNautilus->EvaluateFB(c.Solves, Inspections);
                SearchNautilus->SetTimeFS(c.Time);
                SearchNautilus->SetDepthFS(c.Depth);
                found = true;
                break;
            }
        }

        if (!found) // Solves not found in cache
        {
            Cache.clear(); // Cache memory usage can be large, clear previous cache
            const auto time_fb_start = std::chrono::system_clock::now();

            DeepSearch BaseSearch(Scramble);
            BaseSearch.SearchBase(Depth1, Plc::BEST, NumCores);

            if (BaseSearch.GetSolves().empty())
            {
                emit finishMethod_NoSolves();
                delete SearchNautilus;
                return;
            }

            SearchNautilus->EvaluateFB(BaseSearch.GetSolves(), Inspections);

            const std::chrono::duration<double> fb_elapsed_seconds = std::chrono::system_clock::now() - time_fb_start;
            SearchNautilus->SetTimeFS(fb_elapsed_seconds.count());
            SearchNautilus->SetDepthFS(Depth1);

            // Add current solves into cache only in a full search
            if (!BaseSearch.CheckSkipSearch())
            {
                CacheUnit CU;
                CU.Scramble = Scramble;
                CU.Depth = Depth1;
                CU.Time = SearchNautilus->GetTimeFB();
                CU.Solves = BaseSearch.GetSolves();
                Cache.push_back(CU);
            }
        }
    }
    else // No cache
    {
        CurrentSearchMaxDepthPointer = SearchNautilus->GetMaxDepthPointer();
        if (!SearchNautilus->SearchFB(Depth1, Inspections))
        {
            CurrentSearchMaxDepthPointer = nullptr;
            emit finishMethod_NoSolves();
            delete SearchNautilus;
            return;
        }
        CurrentSearchMaxDepthPointer = nullptr;
        if (FinishingThread) { delete SearchNautilus; return; }
    }

    // Start second blocks search
    emit msgNautilusStartSB();
    CurrentSearchMaxDepthPointer = SearchNautilus->GetMaxDepthPointer();
    SearchNautilus->SearchSB(Depth2);
    CurrentSearchMaxDepthPointer = nullptr;
    if (FinishingThread) { delete SearchNautilus; return; }

    if (VariantIndex == 1) // LSE-NCOLL
    {
        emit msgNautilusStartdFR();
        CurrentSearchMaxDepthPointer = SearchNautilus->GetMaxDepthPointer();
        SearchNautilus->SearchPairdFR(false);
        CurrentSearchMaxDepthPointer = nullptr;
        if (FinishingThread) { delete SearchNautilus; return; }

        emit msgNautilusStartNCOLL();
        SearchNautilus->SearchNCOLL();

        emit msgNautilusStartL5E();
        SearchNautilus->SearchL5E();
    }
    else if (VariantIndex == 2) // LSE-NCLL/TNCLL
    {
        emit msgNautilusStartdFR();
        CurrentSearchMaxDepthPointer = SearchNautilus->GetMaxDepthPointer();
        SearchNautilus->SearchPairdFR(true);
        CurrentSearchMaxDepthPointer = nullptr;
        if (FinishingThread) { delete SearchNautilus; return; }

        emit msgNautilusStartTNCLL();
        SearchNautilus->SearchTNCLL();

        emit msgNautilusStartL5E();
        SearchNautilus->SearchL5E();
    }
    else if (VariantIndex == 3) // LSLL-ZBLL
    {
        emit msgNautilusStartEODF();
        SearchNautilus->SearchEODF();

        emit msgNautilusStartF2L();
        CurrentSearchMaxDepthPointer = SearchNautilus->GetMaxDepthPointer();
        SearchNautilus->SearchF2L();
        CurrentSearchMaxDepthPointer = nullptr;
        if (FinishingThread) { delete SearchNautilus; return; }

        emit msgNautilusStartLL();
        SearchNautilus->SearchZBLL();
    }
    else if (VariantIndex == 4) // LSLL-OCLL/PLL
    {
        emit msgNautilusStartEODF();
        SearchNautilus->SearchEODF();

        emit msgNautilusStartF2L();
        CurrentSearchMaxDepthPointer = SearchNautilus->GetMaxDepthPointer();
        SearchNautilus->SearchF2L();
        CurrentSearchMaxDepthPointer = nullptr;
        if (FinishingThread) { delete SearchNautilus; return; }

        emit msgNautilusStartLL();
        SearchNautilus->SearchOCLL();
        SearchNautilus->SearchPLL();
    }
    else if (VariantIndex == 5) // LSLL-COLL/EPLL
    {
        emit msgNautilusStartEODF();
        SearchNautilus->SearchEODF();

        emit msgNautilusStartF2L();
        CurrentSearchMaxDepthPointer = SearchNautilus->GetMaxDepthPointer();
        SearchNautilus->SearchF2L();
        CurrentSearchMaxDepthPointer = nullptr;
        if (FinishingThread) { delete SearchNautilus; return; }

        emit msgNautilusStartLL();
        SearchNautilus->SearchCOLL();
        SearchNautilus->SearchEPLL();
    }
    else // LSE-NCLL (default)
    {
        emit msgNautilusStartdFR();
        CurrentSearchMaxDepthPointer = SearchNautilus->GetMaxDepthPointer();
        SearchNautilus->SearchPairdFR(false);
        CurrentSearchMaxDepthPointer = nullptr;
        if (FinishingThread) { delete SearchNautilus; return; }

        emit msgNautilusStartNCLL();
        SearchNautilus->SearchNCLL();

        emit msgNautilusStartL5E();
        SearchNautilus->SearchL5E();
    }

    if (RegripsEnabled) SearchNautilus->SetRegrips();

    emit finishMethod_Solves(QString::fromStdString(SearchNautilus->GetTextScramble()),
                             QString::fromStdString(SearchNautilus->GetReport(CancellationsEnabled, false)));
    emit finishMethod_Best(QString::fromStdString(SearchNautilus->GetBestReport(CancellationsEnabled)));
    emit finishMethod_Time(QString::fromStdString(SearchNautilus->GetTimeReport()));

    delete SearchNautilus;
}

  /*****************************************************************************************************************/
 /*** LEOR ********************************************************************************************************/
/*****************************************************************************************************************/

void RunSearch::startSearchLEOR(const QString Scr, const int NumCores, const int BitConfig, const int VariantIndex, const int OptionIndex,
                                const int SpeedIndex, const int OrientIndex, const int AmountIndex, const int MetricIndex)
{
    const Algorithm Scramble(Scr.toStdString());

    const bool CacheEnabled = BitConfig % 2 == 1;
    const bool RegripsEnabled = (BitConfig >> 1) % 2 == 1;
    const bool CancellationsEnabled = (BitConfig >> 2) % 2 == 1;

    // Read the allowed orientations
    std::vector<Spn> SearchSpins;
    uint nSpins = GetSearchSpins(SearchSpins, OrientIndex);

    uint Depth1, Depth2;
    switch (SpeedIndex)
    {
    case 0: Depth1 = 6u; Depth2 = 6u; break;
    case 1: Depth1 = 7u; Depth2 = 6u; break;
    case 2: Depth1 = 7u; Depth2 = 7u; break;
    case 3: Depth1 = 8u; Depth2 = 7u; break;
    default: Depth1 = 6u; Depth2 = 6u; break;
    }

    uint Inspections;
    switch (AmountIndex)
    {
    case 0: Inspections = 1u; break;
    case 1: if (nSpins > 20u) Inspections = 1u; else if (nSpins > 4u) Inspections = 3u; else Inspections = 6u; break;
    case 2: if (nSpins > 20u) Inspections = 2u; else if (nSpins > 4u) Inspections = 6u; else Inspections = 12u; break;
    case 3: if (nSpins > 20u) Inspections = 4u; else if (nSpins > 4u) Inspections = 12u; else Inspections = 24u; break;
    case 4: if (nSpins > 20u) Inspections = 8u; else if (nSpins > 4u) Inspections = 24u; else Inspections = 48u; break;
    default: Inspections = 1u; break;
    }

    // Create search
    LEOR* SearchLEOR = new LEOR(Scramble, NumCores);

    SearchLEOR->SetSearchSpins(SearchSpins);

    SearchLEOR->SetMetric(static_cast<Metrics>(MetricIndex));

    // Start first blocks search
    emit msgLEORStartFB();

    if (CacheEnabled)
    {
        bool found = false;
        for (const auto& c : Cache) // Search in cache
        {
            if (c.Scramble == Scramble && c.Depth >= Depth1 && !c.Solves.empty())
            {
                emit msgCache();
                if (VariantIndex == 1) SearchLEOR->EvaluateFBB(c.Solves, Inspections);
                else SearchLEOR->EvaluateFBA(c.Solves, Inspections);
                SearchLEOR->SetTimeFS(c.Time);
                SearchLEOR->SetDepthFS(c.Depth);
                found = true;
                break;
            }
        }

        if (!found) // Solves not found in cache
        {
            Cache.clear(); // Cache memory usage can be large, clear previous cache
            const auto time_fb_start = std::chrono::system_clock::now();

            DeepSearch BaseSearch(Scramble);
            BaseSearch.SearchBase(Depth1, Plc::BEST, NumCores);

            if (BaseSearch.GetSolves().empty())
            {
                emit finishMethod_NoSolves();
                delete SearchLEOR;
                return;
            }

            if (VariantIndex == 1) SearchLEOR->EvaluateFBB(BaseSearch.GetSolves(), Inspections);
            else SearchLEOR->EvaluateFBA(BaseSearch.GetSolves(), Inspections);

            const std::chrono::duration<double> fb_elapsed_seconds = std::chrono::system_clock::now() - time_fb_start;
            SearchLEOR->SetTimeFS(fb_elapsed_seconds.count());
            SearchLEOR->SetDepthFS(Depth1);

            // Add current solves into cache only in a full search
            if (!BaseSearch.CheckSkipSearch())
            {
                CacheUnit CU;
                CU.Scramble = Scramble;
                CU.Depth = Depth1;
                CU.Time = SearchLEOR->GetTimeFB();
                CU.Solves = BaseSearch.GetSolves();
                Cache.push_back(CU);
            }
        }
    }
    else // No cache
    {
        CurrentSearchMaxDepthPointer = SearchLEOR->GetMaxDepthPointer();
        if ((VariantIndex == 1 && !SearchLEOR->SearchFBB(Depth1, Inspections)) || // LEOR-B
            (VariantIndex != 1 && !SearchLEOR->SearchFBA(Depth1, Inspections)))   // LEOR-A
        {
            CurrentSearchMaxDepthPointer = nullptr;
            emit finishMethod_NoSolves();
            delete SearchLEOR;
            return;
        }
        CurrentSearchMaxDepthPointer = nullptr;
        if (FinishingThread) { delete SearchLEOR; return; }
    }

    if (VariantIndex == 1) // LEOR-B
    {
        emit msgLEORStartFLPair();
        CurrentSearchMaxDepthPointer = SearchLEOR->GetMaxDepthPointer();
        SearchLEOR->SearchFLPair();
        CurrentSearchMaxDepthPointer = nullptr;
        if (FinishingThread) { delete SearchLEOR; return; }

        emit msgLEORStartEODF();
        CurrentSearchMaxDepthPointer = SearchLEOR->GetMaxDepthPointer();
        SearchLEOR->SearchEOStripe(Depth2);
        CurrentSearchMaxDepthPointer = nullptr;
        if (FinishingThread) { delete SearchLEOR; return; }
    }
    else // if (VariantIndex == 0) // LEOR-A
    {
        emit msgLEORStartEOStripe();
        CurrentSearchMaxDepthPointer = SearchLEOR->GetMaxDepthPointer();
        SearchLEOR->SearchEOStripe(Depth2);
        CurrentSearchMaxDepthPointer = nullptr;
        if (FinishingThread) { delete SearchLEOR; return; }
    }

    // Second block
    emit msgLEORStartSB();
    CurrentSearchMaxDepthPointer = SearchLEOR->GetMaxDepthPointer();
    SearchLEOR->SearchSB();
    CurrentSearchMaxDepthPointer = nullptr;
    if (FinishingThread) { delete SearchLEOR; return; }

    // Last layer
    emit msgLEORStartLL();
    if (OptionIndex == 1)
    {
        SearchLEOR->SearchOCLL();
        SearchLEOR->SearchPLL();
    }
    else if (OptionIndex == 2)
    {
        SearchLEOR->SearchCOLL();
        SearchLEOR->SearchEPLL();
    }
    else // OptionIndex == 0
    {
        SearchLEOR->SearchZBLL();
    }

    if (RegripsEnabled) SearchLEOR->SetRegrips();

    emit finishMethod_Solves(QString::fromStdString(SearchLEOR->GetTextScramble()),
                             QString::fromStdString(SearchLEOR->GetReport(CancellationsEnabled, false)));
    emit finishMethod_Best(QString::fromStdString(SearchLEOR->GetBestReport(CancellationsEnabled)));
    emit finishMethod_Time(QString::fromStdString(SearchLEOR->GetTimeReport()));

    delete SearchLEOR;
}

// Get the search spins from the orientation
uint RunSearch::GetSearchSpins(std::vector<Spn>& SS, const int OrientIndex) // static
{
    SS.clear();

    switch (OrientIndex)
    {
    case 0: for (int s = 0; s < 24; s++) SS.push_back(static_cast<Spn>(s)); break; // All
    case 1: SS.push_back(Spn::UF); SS.push_back(Spn::UB); SS.push_back(Spn::UR); SS.push_back(Spn::UL);
            SS.push_back(Spn::DF); SS.push_back(Spn::DB); SS.push_back(Spn::DR); SS.push_back(Spn::DL); break;
    case 2: SS.push_back(Spn::FU); SS.push_back(Spn::FD); SS.push_back(Spn::FR); SS.push_back(Spn::FL);
            SS.push_back(Spn::BU); SS.push_back(Spn::BD); SS.push_back(Spn::BR); SS.push_back(Spn::BL); break;
    case 3: SS.push_back(Spn::RU); SS.push_back(Spn::RD); SS.push_back(Spn::RF); SS.push_back(Spn::RB);
            SS.push_back(Spn::LU); SS.push_back(Spn::LD); SS.push_back(Spn::LF); SS.push_back(Spn::LB); break;
    case 4: SS.push_back(Spn::UF); SS.push_back(Spn::UB); SS.push_back(Spn::UR); SS.push_back(Spn::UL); break;
    case 5: SS.push_back(Spn::DF); SS.push_back(Spn::DB); SS.push_back(Spn::DR); SS.push_back(Spn::DL); break;
    case 6: SS.push_back(Spn::FU); SS.push_back(Spn::FD); SS.push_back(Spn::FR); SS.push_back(Spn::FL); break;
    case 7: SS.push_back(Spn::BU); SS.push_back(Spn::BD); SS.push_back(Spn::BR); SS.push_back(Spn::BL); break;
    case 8: SS.push_back(Spn::RU); SS.push_back(Spn::RD); SS.push_back(Spn::RF); SS.push_back(Spn::RB); break;
    case 9: SS.push_back(Spn::LU); SS.push_back(Spn::LD); SS.push_back(Spn::LF); SS.push_back(Spn::LB); break;
    default: SS.push_back(Spn::UF); break;
    }

    return static_cast<uint>(SS.size());
}

// Get the cross layers from the orientation (CFOP)
uint RunSearch::GetCrossLayers(std::vector<Lyr>& CL, int OrientIndex)
{
    CL.clear();

    switch (OrientIndex)
    {
    case 0: CL.push_back(Lyr::U); CL.push_back(Lyr::D);
            CL.push_back(Lyr::F); CL.push_back(Lyr::B);
            CL.push_back(Lyr::R); CL.push_back(Lyr::L); break;
    case 1: CL.push_back(Lyr::U); CL.push_back(Lyr::D); break;
    case 2: CL.push_back(Lyr::F); CL.push_back(Lyr::B); break;
    case 3: CL.push_back(Lyr::R); CL.push_back(Lyr::L); break;
    case 4: CL.push_back(Lyr::D); break;
    case 5: CL.push_back(Lyr::U); break;
    case 6: CL.push_back(Lyr::B); break;
    case 7: CL.push_back(Lyr::F); break;
    case 8: CL.push_back(Lyr::L); break;
    case 9: CL.push_back(Lyr::R); break;
    default: CL.push_back(Lyr::U); break;
    }

    return static_cast<uint>(CL.size());
}
