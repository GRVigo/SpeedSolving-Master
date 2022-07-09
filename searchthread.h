#ifndef SEARCHTHREAD_H
#define SEARCHTHREAD_H

#include <QThread>

#include "algorithm.h"

using namespace grcube3;

class RunSearch : public QObject
{
   Q_OBJECT
public:
   RunSearch() : QObject()
   {
       FinishingThread = false;
       CurrentSearchMaxDepthPointer = nullptr;
       moveToThread(&t);
       t.start();
   }

   ~RunSearch()
   {
       FinishingThread = true;
       Skip();
       t.quit();
       t.wait();
   }

   void Skip() { if (CurrentSearchMaxDepthPointer != nullptr) *CurrentSearchMaxDepthPointer = 0u; }

   // Layer by layer search
   void doStartSearchLBL(const QString Scramble, const int CrossLayerIndex, const int NumCores, const int MetricIndex)
   {
      QMetaObject::invokeMethod(this, "startSearchLBL",
                                Q_ARG(QString, Scramble),
                                Q_ARG(int, CrossLayerIndex),
                                Q_ARG(int, NumCores),
                                Q_ARG(int, MetricIndex));
   }

   // CFOP search
   void doStartSearchCFOP(const QString Scramble,
                          const int NumCores,
                          const bool CacheEnabled,
                          const int VariantIndex,
                          const int OptionIndex,
                          const int SpeedIndex,
                          const int OrientIndex,
                          const int AmountIndex,
                          const int MetricIndex,
                          const bool RegripsEnabled,
                          const bool CancellationsEnabled)
   {
       int BitConfig = 0;
       if (CacheEnabled) BitConfig += 1;
       if (RegripsEnabled) BitConfig += 2;
       if (CancellationsEnabled) BitConfig += 4;

       QMetaObject::invokeMethod(this, "startSearchCFOP",
                                 Q_ARG(QString, Scramble),
                                 Q_ARG(int, NumCores),
                                 Q_ARG(int, BitConfig),
                                 Q_ARG(int, VariantIndex),
                                 Q_ARG(int, OptionIndex),
                                 Q_ARG(int, SpeedIndex),
                                 Q_ARG(int, OrientIndex),
                                 Q_ARG(int, AmountIndex),
                                 Q_ARG(int, MetricIndex));
   }
   
   // Roux search
   void doStartSearchRoux(const QString Scramble,
                          const int NumCores,
                          const bool CacheEnabled,
                          const int VariantIndex,
						  const int OptionIndex,
                          const int SpeedIndex,
                          const int OrientIndex,
                          const int AmountIndex,
                          const int MetricIndex,
                          const bool RegripsEnabled,
                          const bool CancellationsEnabled)
   {
       int BitConfig = 0;
       if (CacheEnabled) BitConfig += 1;
       if (RegripsEnabled) BitConfig += 2;
       if (CancellationsEnabled) BitConfig += 4;

       QMetaObject::invokeMethod(this, "startSearchRoux",
                                 Q_ARG(QString, Scramble),
                                 Q_ARG(int, NumCores),
                                 Q_ARG(int, BitConfig),
                                 Q_ARG(int, VariantIndex),
								 Q_ARG(int, OptionIndex),
                                 Q_ARG(int, SpeedIndex),
                                 Q_ARG(int, OrientIndex),
                                 Q_ARG(int, AmountIndex),
                                 Q_ARG(int, MetricIndex));
   }

   // Petrus search
   void doStartSearchPetrus(const QString Scramble,
                            const int NumCores,
                            const bool CacheEnabled,
                            const int VariantIndex,
                            const int OptionIndex,
                            const int SpeedIndex,
                            const int OrientIndex,
                            const int AmountIndex,
                            const int MetricIndex,
                            const bool RegripsEnabled,
                            const bool CancellationsEnabled)
   {
       int BitConfig = 0;
       if (CacheEnabled) BitConfig += 1;
       if (RegripsEnabled) BitConfig += 2;
       if (CancellationsEnabled) BitConfig += 4;

       QMetaObject::invokeMethod(this, "startSearchPetrus",
                                 Q_ARG(QString, Scramble),
                                 Q_ARG(int, NumCores),
                                 Q_ARG(int, BitConfig),
                                 Q_ARG(int, VariantIndex),
                                 Q_ARG(int, OptionIndex),
                                 Q_ARG(int, SpeedIndex),
                                 Q_ARG(int, OrientIndex),
                                 Q_ARG(int, AmountIndex),
                                 Q_ARG(int, MetricIndex));
   }

   // ZZ search
   void doStartSearchZZ(const QString Scramble,
                        const int NumCores,
                        const bool CacheEnabled,
                        const int VariantIndex,
                        const int OptionIndex,
                        const int SpeedIndex,
                        const int OrientIndex,
                        const int AmountIndex,
                        const int MetricIndex,
                        const bool RegripsEnabled,
                        const bool CancellationsEnabled)
   {
       int BitConfig = 0;
       if (CacheEnabled) BitConfig += 1;
       if (RegripsEnabled) BitConfig += 2;
       if (CancellationsEnabled) BitConfig += 4;

       QMetaObject::invokeMethod(this, "startSearchZZ",
                                 Q_ARG(QString, Scramble),
                                 Q_ARG(int, NumCores),
                                 Q_ARG(int, BitConfig),
                                 Q_ARG(int, VariantIndex),
                                 Q_ARG(int, OptionIndex),
                                 Q_ARG(int, SpeedIndex),
                                 Q_ARG(int, OrientIndex),
                                 Q_ARG(int, AmountIndex),
                                 Q_ARG(int, MetricIndex));
   }

   // CEOR search
   void doStartSearchCEOR(const QString Scramble,
                          const int NumCores,
                          const bool CacheEnabled,
                          const int VariantIndex,
                          const int OptionIndex,
                          const int SpeedIndex,
                          const int OrientIndex,
                          const int AmountIndex,
                          const int MetricIndex,
                          const bool RegripsEnabled,
                          const bool CancellationsEnabled)
   {
       int BitConfig = 0;
       if (CacheEnabled) BitConfig += 1;
       if (RegripsEnabled) BitConfig += 2;
       if (CancellationsEnabled) BitConfig += 4;

       QMetaObject::invokeMethod(this, "startSearchCEOR",
                                 Q_ARG(QString, Scramble),
                                 Q_ARG(int, NumCores),
                                 Q_ARG(int, BitConfig),
                                 Q_ARG(int, VariantIndex),
                                 Q_ARG(int, OptionIndex),
                                 Q_ARG(int, SpeedIndex),
                                 Q_ARG(int, OrientIndex),
                                 Q_ARG(int, AmountIndex),
                                 Q_ARG(int, MetricIndex));
   }

   // Mehta search
   void doStartSearchMehta(const QString Scramble,
                           const int NumCores,
                           const bool CacheEnabled,
                           const int VariantIndex,
                           const int OptionIndex,
                           const int SpeedIndex,
                           const int OrientIndex,
                           const int AmountIndex,
                           const int MetricIndex,
                           const bool RegripsEnabled,
                           const bool CancellationsEnabled)
   {
       int BitConfig = 0;
       if (CacheEnabled) BitConfig += 1;
       if (RegripsEnabled) BitConfig += 2;
       if (CancellationsEnabled) BitConfig += 4;

       QMetaObject::invokeMethod(this, "startSearchMehta",
                                 Q_ARG(QString, Scramble),
                                 Q_ARG(int, NumCores),
                                 Q_ARG(int, BitConfig),
                                 Q_ARG(int, VariantIndex),
                                 Q_ARG(int, OptionIndex),
                                 Q_ARG(int, SpeedIndex),
                                 Q_ARG(int, OrientIndex),
                                 Q_ARG(int, AmountIndex),
                                 Q_ARG(int, MetricIndex));
   }

   // Nautilus search
   void doStartSearchNautilus(const QString Scramble,
                              const int NumCores,
                              const bool CacheEnabled,
                              const int VariantIndex,
                              const int OptionIndex,
                              const int SpeedIndex,
                              const int OrientIndex,
                              const int AmountIndex,
                              const int MetricIndex,
                              const bool RegripsEnabled,
                              const bool CancellationsEnabled)
   {
       int BitConfig = 0;
       if (CacheEnabled) BitConfig += 1;
       if (RegripsEnabled) BitConfig += 2;
       if (CancellationsEnabled) BitConfig += 4;

       QMetaObject::invokeMethod(this, "startSearchNautilus",
                                 Q_ARG(QString, Scramble),
                                 Q_ARG(int, NumCores),
                                 Q_ARG(int, BitConfig),
                                 Q_ARG(int, VariantIndex),
                                 Q_ARG(int, OptionIndex),
                                 Q_ARG(int, SpeedIndex),
                                 Q_ARG(int, OrientIndex),
                                 Q_ARG(int, AmountIndex),
                                 Q_ARG(int, MetricIndex));
   }

   // LEOR search
   void doStartSearchLEOR(const QString Scramble,
                          const int NumCores,
                          const bool CacheEnabled,
                          const int VariantIndex,
                          const int OptionIndex,
                          const int SpeedIndex,
                          const int OrientIndex,
                          const int AmountIndex,
                          const int MetricIndex,
                          const bool RegripsEnabled,
                          const bool CancellationsEnabled)
   {
       int BitConfig = 0;
       if (CacheEnabled) BitConfig += 1;
       if (RegripsEnabled) BitConfig += 2;
       if (CancellationsEnabled) BitConfig += 4;

       QMetaObject::invokeMethod(this, "startSearchLEOR",
                                 Q_ARG(QString, Scramble),
                                 Q_ARG(int, NumCores),
                                 Q_ARG(int, BitConfig),
                                 Q_ARG(int, VariantIndex),
                                 Q_ARG(int, OptionIndex),
                                 Q_ARG(int, SpeedIndex),
                                 Q_ARG(int, OrientIndex),
                                 Q_ARG(int, AmountIndex),
                                 Q_ARG(int, MetricIndex));
   }

   // static functions
   static uint GetSearchSpins(std::vector<grcube3::Spn>&, const int);
   static uint GetCrossLayers(std::vector<Lyr>&, int);

public slots:
    void startSearchLBL(const QString, const int, const int, const int);
    void startSearchCFOP(const QString, const int, const int, const int, const int, const int, const int, const int, const int);
    void startSearchRoux(const QString, const int, const int, const int, const int, const int, const int, const int, const int);
    void startSearchPetrus(const QString, const int, const int, const int, const int, const int, const int, const int, const int);
    void startSearchZZ(const QString, const int, const int, const int, const int, const int, const int, const int, const int);
    void startSearchCEOR(const QString, const int, const int, const int, const int, const int, const int, const int, const int);
    void startSearchMehta(const QString, const int, const int, const int, const int, const int, const int, const int, const int);
    void startSearchNautilus(const QString, const int, const int, const int, const int, const int, const int, const int, const int);
    void startSearchLEOR(const QString, const int, const int, const int, const int, const int, const int, const int, const int);

signals:
    void msgLBLStartCross();
    void msgLBLStartFL();
    void msgLBLStartSL();
    void msgLBLStartLL();
    void finishLBL_Solves(const QString, const QString, const QString); // Scramble, Cross layer, Solve
    void finishLBL_NoSolves();

    void finishMethod_Solves(const QString, const QString); // Scramble, Solves
    void finishMethod_Best(const QString); // Best solve
    void finishMethod_Time(const QString);
    void finishMethod_NoSolves();

    void msgCFOPStartCross();
    void msgCFOPStartF2L();
    void msgCFOPStartLL();

    void msgRouxStartFB();
    void msgRouxStartSB();
    void msgRouxStartCMLL();
    void msgRouxStartCOLL();
    void msgRouxStartL6E();

    void msgPetrusStartFB();
    void msgPetrusStartEB();
    void msgPetrusStartEO();
    void msgPetrusStartF2L();
    void msgPetrusStartLL();

    void msgZZStartEOX();
    void msgZZStartF2L();
    void msgZZStartLL();

    void msgCEORStartLines();
    void msgCEORStartCP();
    void msgCEORStartCPLines();
    void msgCEORStartpEO();
    void msgCEORStartEOBF();
    void msgCEORStartF2L();
    void msgCEORStartLL();

    void msgMehtaStartFB();
    void msgMehtaStart3QB();
    void msgMehtaStartEOLE();
    void msgMehtaStart6CO();
    void msgMehtaStart6CP();
    void msgMehtaStartL5EP();
    void msgMehtaStartAPDR();
    void msgMehtaStartPLL();
    void msgMehtaStartDCAL();
    void msgMehtaStartCDRLL();
    void msgMehtaStartJTLE();
    void msgMehtaStartTDR();
    void msgMehtaStartZBLL();

    void msgNautilusStartFB();
    void msgNautilusStartSB();
    void msgNautilusStartdFR();
    void msgNautilusStartNCLL();
    void msgNautilusStartNCOLL();
    void msgNautilusStartTNCLL();
    void msgNautilusStartL5E();
    void msgNautilusStartEODF();
    void msgNautilusStartF2L();
    void msgNautilusStartLL();

    void msgLEORStartFB();
    void msgLEORStartEOStripe();
    void msgLEORStartFLPair();
    void msgLEORStartEODF();
    void msgLEORStartSB();
    void msgLEORStartLL();

    void msgCache();

private:
   QThread t;

    uint* CurrentSearchMaxDepthPointer;

    bool FinishingThread;

   // Information to store about a solve search for the cache
   struct CacheUnit
   {
       Algorithm Scramble;
       uint Depth;
       double Time;
       std::vector<Algorithm> Solves;

       void Reset() { Scramble.Clear(); Depth = 0u; Time = 0.0; Solves.clear(); }
   };

   std::vector<CacheUnit> Cache; // Solves cache
};


#endif // SEARCHTHREAD_H
