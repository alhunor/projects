#include <string>
#include <conio.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include "..\mystuff\hftimer.h"
#include "..\mystuff\hash.h"
#include "fsa.h"


struct VFNamesWithHashCode
{
	char* vfname;
	FUNCPTR func;
	int hashcode;
} hashtable[324];

std::unordered_map<int, FUNCPTR> mappy;

void accept()
{
	volatile int a=1;
}


void smartass(char * str, fsa& automata)
{
	void(*funcptr)();
	funcptr = automata.matchPattern1(str);
	//funcptr = automata.match(str);
	funcptr();
}

void dumbass(char * str, fsa& automata)
{
	if (_stricmp(str, "ABSCDSBasketTranche") == 0) { accept(); return;} 
	if (_stricmp(str,"ABSCreditDefaultSwap") == 0) { accept(); return;}
	if (_stricmp(str,"AlgorithmiclndexBasketTotalReturn") == 0) { accept(); return;}
	if (_stricmp(str,"AlgorithmicIndexBasketTotalReturn2") == 0) { accept(); return;}
	if (_stricmp(str,"AlgorithmicIndexBasketTotalReturnOption") == 0) { accept(); return;}
	if (_stricmp(str,"AlgorithmicIndexLiborOption") == 0) { accept(); return;}
	if (_stricmp(str,"AlgorithmicIndexTotalReturn") == 0) { accept(); return;}
	if (_stricmp(str,"AlgorithmicIndexTotalReturnOption") == 0) { accept(); return;}
	if (_stricmp(str,"AnnuityOption3") == 0) { accept(); return;}
	if (_stricmp(str,"AsianDigitalQCMSLeg") == 0) { accept(); return;}
	if (_stricmp(str,"AsianSpreadOption") == 0) { accept(); return;}
	if (_stricmp(str,"AutoCap") == 0) { accept(); return;}
	if (_stricmp(str,"BarrierPRD") == 0) { accept(); return;}
	if (_stricmp(str,"BarrierPRD2") == 0) { accept(); return;}
	if (_stricmp(str,"BarrierSwap") == 0) { accept(); return;}
	if (_stricmp(str,"BasisSwap") == 0) { accept(); return;}
	if (_stricmp(str,"BasisSwapLegUntilDefault") == 0) { accept(); return;}
	if (_stricmp(str,"BasisSwapMtM") == 0) { accept(); return;}
	if (_stricmp(str,"BasisSwapMtM2") == 0) { accept(); return;}
	if (_stricmp(str,"BasisSwaption") == 0) { accept(); return;}
	if (_stricmp(str,"BermudanAsianSpreadOption") == 0) { accept(); return;}
	if (_stricmp(str,"BermudanCDOCorridor") == 0) { accept(); return;}
	if (_stricmp(str,"BermudanCorridor") == 0) { accept(); return;}
	if (_stricmp(str,"BermudanDoubleEquityCorridor") == 0) { accept(); return;}
	if (_stricmp(str,"BermudanDoubleSpreadCorridor") == 0) { accept(); return;}
	if (_stricmp(str,"BermudanQuantoCorridor") == 0) { accept(); return;}
	if (_stricmp(str,"BermudanSpreadCorridor") == 0) { accept(); return;}
	if (_stricmp(str,"BermudanSwaption") == 0) { accept(); return;}
	if (_stricmp(str,"BermudanSwaption2") == 0) { accept(); return;}
	if (_stricmp(str,"BermudanSwaption2BondLinked") == 0) { accept(); return;}
	if (_stricmp(str,"BermudanZeroSwaption") == 0) { accept(); return;}
	if (_stricmp(str,"BestOflnflationAndEquity") == 0) { accept(); return;}
	if (_stricmp(str,"BondASMOption") == 0) { accept(); return;}
	if (_stricmp(str,"BondCashFlowsTrade") == 0) { accept(); return;}
	if (_stricmp(str,"BondExchangeableASMOption") == 0) { accept(); return;}
	if (_stricmp(str,"BondFixedASMOption") == 0) { accept(); return;}
	if (_stricmp(str,"BondFutureOptionTrade") == 0) { accept(); return;}
	if (_stricmp(str,"BondFutureTrade") == 0) { accept(); return;}
	if (_stricmp(str,"BondGapOption") == 0) { accept(); return;}
	if (_stricmp(str,"BondOption") == 0) { accept(); return;}
	if (_stricmp(str,"BondOptionTrade") == 0) { accept(); return;}
	if (_stricmp(str,"BondRepo") == 0) { accept(); return;}
	if (_stricmp(str,"BondRepoCustom") == 0) { accept(); return;}
	if (_stricmp(str,"BondTrade") == 0) { accept(); return;}
	if (_stricmp(str,"BradyBondTrade") == 0) { accept(); return;}
	if (_stricmp(str,"BradyFRNTrade") == 0) { accept(); return;}
	if (_stricmp(str,"CallableBondAssetSwap") == 0) { accept(); return;}
	if (_stricmp(str,"CallableCDOCorridor") == 0) { accept(); return;}
	if (_stricmp(str,"CallableCorridor") == 0) { accept(); return;}
	if (_stricmp(str,"CallableCorridorBondLinked") == 0) { accept(); return;}
	if (_stricmp(str,"CallableCreditDefaultSwap") == 0) { accept(); return;}
	if (_stricmp(str,"CallableDoubleSpreadCorridor") == 0) { accept(); return;}
	if (_stricmp(str,"CallableDualReverseFloater") == 0) { accept(); return;}
	if (_stricmp(str,"CallableDualSpread") == 0) { accept(); return;}
	if (_stricmp(str,"CallableEquityOption") == 0) { accept(); return;}
	if (_stricmp(str,"CallableFxIRSpreadOption") == 0) { accept(); return;}
	if (_stricmp(str,"CallableFxTrade") == 0) { accept(); return;}
	if (_stricmp(str,"CallableLiborTrade") == 0) { accept(); return;}
	if (_stricmp(str,"CallablePRD") == 0) { accept(); return;}
	if (_stricmp(str,"CallableQuantoCorridor") == 0) { accept(); return;}
	if (_stricmp(str,"CallableRatchetCorridor") == 0) { accept(); return;}
	if (_stricmp(str,"CallableRepoTrade") == 0) { accept(); return;}
	if (_stricmp(str,"CallableReverseFloater") == 0) { accept(); return;}
	if (_stricmp(str,"CallableRiskyAssetSwap") == 0) { accept(); return;}
	if (_stricmp(str,"CallableRiskyFloatingAssetSwap") == 0) { accept(); return;}
	if (_stricmp(str,"CallableRiskySpreadOption") == 0) { accept(); return;}
	if (_stricmp(str,"CallableSpreadCorridor") == 0) { accept(); return;}
	if (_stricmp(str,"CallableSpreadOption") == 0) { accept(); return;}
	if (_stricmp(str,"CapFloor") == 0) { accept(); return;}
	if (_stricmp(str,"CashBalance") == 0) { accept(); return;}
	if (_stricmp(str,"CashlndexOption") == 0) { accept(); return;}
	if (_stricmp(str,"CD0Forward") == 0) { accept(); return;}
	if (_stricmp(str,"CDOofCDOs") == 0) { accept(); return;}
	if (_stricmp(str,"CDSBasketIndex") == 0) { accept(); return;}
	if (_stricmp(str,"CDSBasketIndexCapFloor") == 0) { accept(); return;}
	if (_stricmp(str,"CDSBasketIndexCMSLeg") == 0) { accept(); return;}
	if (_stricmp(str,"CDSBasketIndexOption") == 0) { accept(); return;}
	if (_stricmp(str,"CDSBasketRollingIndexCapFloor") == 0) { accept(); return;}
	if (_stricmp(str,"CDSBasketRollingIndexCMSLeg") == 0) { accept(); return;}
	if (_stricmp(str,"CDSBasketTranche") == 0) { accept(); return;}
	if (_stricmp(str,"CDSBasketTranchProtectionTrade") == 0) { accept(); return;}
	if (_stricmp(str,"CDSPortfolio") == 0) { accept(); return;}
	if (_stricmp(str,"ChooserTarn") == 0) { accept(); return;}
	if (_stricmp(str,"CMMForwardRateAgreement") == 0) { accept(); return;}
	if (_stricmp(str,"CMMLeg") == 0) { accept(); return;}
	if (_stricmp(str,"CMSDualRateOption") == 0) { accept(); return;}
	if (_stricmp(str,"CMSLinkedIRRSwap") == 0) { accept(); return;}
	if (_stricmp(str,"CMSMultiplier") == 0) { accept(); return;}
	if (_stricmp(str,"CMSSpreadHedged") == 0) { accept(); return;}
	if (_stricmp(str,"CMSTripleRateOption") == 0) { accept(); return;}
	if (_stricmp(str,"CMTCapFloor") == 0) { accept(); return;}
	if (_stricmp(str,"CMTLeg") == 0) { accept(); return;}
	if (_stricmp(str,"CMTLeg2") == 0) { accept(); return;}
	if (_stricmp(str,"CommodityAPO") == 0) { accept(); return;}
	if (_stricmp(str,"CommodityBasisSwap") == 0) { accept(); return;}
	if (_stricmp(str,"CommodityBasketOption") == 0) { accept(); return;}
	if (_stricmp(str,"CommodityBasketSwap") == 0) { accept(); return;}
	if (_stricmp(str,"CommodityClearedAPO") == 0) { accept(); return;}
	if (_stricmp(str,"CommodityClearedBasisSwap") == 0) { accept(); return;}
	if (_stricmp(str,"CommodityClearedGasForward") == 0) { accept(); return;}
	if (_stricmp(str,"CommodityClearedPowerForward") == 0) { accept(); return;}
	if (_stricmp(str,"CommodityClearedSwap") == 0) { accept(); return;}
	if (_stricmp(str,"CommodityCompoAPO") == 0) { accept(); return;}
	if (_stricmp(str,"CommodityCompoSwap") == 0) { accept(); return;}
	if (_stricmp(str,"CommodityDerivedInstrumentPrice") == 0) { accept(); return;}
	if (_stricmp(str,"CommodityEnergyOption") == 0) { accept(); return;}
	if (_stricmp(str,"CommodityFloatingForward") == 0) { accept(); return;}
	if (_stricmp(str,"CommodityFuture") == 0) { accept(); return;}
	if (_stricmp(str,"CommodityFutureOption") == 0) { accept(); return;}
	if (_stricmp(str,"CommodityFutureOptionTrade") == 0) { accept(); return;}
	if (_stricmp(str,"CommodityFutureTrade") == 0) { accept(); return;}
	if (_stricmp(str,"CommodityGasForward") == 0) { accept(); return;}
	if (_stricmp(str,"CommodityGasStorage") == 0) { accept(); return;}
	if (_stricmp(str,"Commodity0ption") == 0) { accept(); return;}
	if (_stricmp(str,"Commodity0ptionTrade") == 0) { accept(); return;}
	if (_stricmp(str,"CommodityPowerForward") == 0) { accept(); return;}
	if (_stricmp(str,"CommoditySwap") == 0) { accept(); return;}
	if (_stricmp(str,"CommodityTimeSpread") == 0) { accept(); return;}
	if (_stricmp(str,"CommodityTrade") == 0) { accept(); return;}
	if (_stricmp(str,"CompoundingFixedLeg") == 0) { accept(); return;}
	if (_stricmp(str,"ContingentCDS") == 0) { accept(); return;}
	if (_stricmp(str,"ConvertibleBondAssetSwap") == 0) { accept(); return;}
	if (_stricmp(str,"ConvertibleBondAssetSwap2") == 0) { accept(); return;}
	if (_stricmp(str,"ConvertibleBondOption") == 0) { accept(); return;}
	if (_stricmp(str,"CorridorCapFloor") == 0) { accept(); return;}
	if (_stricmp(str,"CorridorChooser") == 0) { accept(); return;}
	if (_stricmp(str,"CorridorCMSSpread") == 0) { accept(); return;}
	if (_stricmp(str,"CorridorCHT") == 0) { accept(); return;}
	if (_stricmp(str,"CorridorCommodity") == 0) { accept(); return;}
	if (_stricmp(str,"CorridorCommodityFuture") == 0) { accept(); return;}
	if (_stricmp(str,"CorridorCommodityNew") == 0) { accept(); return;}
	if (_stricmp(str,"CorridorCommodityOld") == 0) { accept(); return;}
	if (_stricmp(str,"CorridorCredit") == 0) { accept(); return;}
	if (_stricmp(str,"CorridorCreditBasket") == 0) { accept(); return;}
	if (_stricmp(str,"CorridorDoubleCommodityRate") == 0) { accept(); return;}
	if (_stricmp(str,"CorridorDoubleEquityRate") == 0) { accept(); return;}
	if (_stricmp(str,"CorridorDoubleSpread") == 0) { accept(); return;}
	if (_stricmp(str,"CorridorEquity") == 0) { accept(); return;}
	if (_stricmp(str,"CorridorFX") == 0) { accept(); return;}
	if (_stricmp(str,"CorridorFxChooser") == 0) { accept(); return;}
	if (_stricmp(str,"CorridorGenericFuture") == 0) { accept(); return;}
	if (_stricmp(str,"CorridorQChooser") == 0) { accept(); return;}
	if (_stricmp(str,"CorridorQSpread") == 0) { accept(); return;}
	if (_stricmp(str,"CorridorQuanto") == 0) { accept(); return;}
	if (_stricmp(str,"CorridorSpread") == 0) { accept(); return;}
	if (_stricmp(str,"CorridorTrade") == 0) { accept(); return;}
	if (_stricmp(str,"CouponChooserPRD") == 0) { accept(); return;}
	if (_stricmp(str,"CPRDChooser") == 0) { accept(); return;}
	if (_stricmp(str,"CreditCMSLeg") == 0) { accept(); return;}
	if (_stricmp(str,"CreditCPDO") == 0) { accept(); return;}
	if (_stricmp(str,"CreditCPPI") == 0) { accept(); return;}
	if (_stricmp(str,"CreditDefaultSwap") == 0) { accept(); return;}
	if (_stricmp(str,"CreditDefaultSwapOption") == 0) { accept(); return;}
	if (_stricmp(str,"CreditFuture") == 0) { accept(); return;}
	if (_stricmp(str,"CreditIndexBasisLimitOption") == 0) { accept(); return;}
	if (_stricmp(str,"CreditRecoveryLock") == 0) { accept(); return;}
	if (_stricmp(str,"CreditZeroCouponBasket") == 0) { accept(); return;}
	if (_stricmp(str,"CrossCurrencySwap") == 0) { accept(); return;}
	if (_stricmp(str,"CrossCurrencySwapFixed") == 0) { accept(); return;}
	if (_stricmp(str,"CrossCurrencySwapFloating") == 0) { accept(); return;}
	if (_stricmp(str,"CrossCurrencySwaption") == 0) { accept(); return;}
	if (_stricmp(str,"CVAFromExposures") == 0) { accept(); return;}
	if (_stricmp(str,"CVAPortfolio") == 0) { accept(); return;}
	if (_stricmp(str,"CVARiskyPortfolio") == 0) { accept(); return;}
	if (_stricmp(str,"DepositLoan") == 0) { accept(); return;}
	if (_stricmp(str,"DualCcyBondTrade") == 0) { accept(); return;}
	if (_stricmp(str,"EquityDividendSwap") == 0) { accept(); return;}
	if (_stricmp(str,"EquityFuture") == 0) { accept(); return;}
	if (_stricmp(str,"EquityFutureOption") == 0) { accept(); return;}
	if (_stricmp(str,"EquityIndexTotalReturn") == 0) { accept(); return;}
	if (_stricmp(str,"EquityOption") == 0) { accept(); return;}
	if (_stricmp(str,"Equity0ption2") == 0) { accept(); return;}
	if (_stricmp(str,"Equity0ptionSwap") == 0) { accept(); return;}
	if (_stricmp(str,"EquitySwap") == 0) { accept(); return;}
	if (_stricmp(str,"EquityTrade") == 0) { accept(); return;}
	if (_stricmp(str,"EquityVarSwap") == 0) { accept(); return;}
	if (_stricmp(str,"EquivalentVanillaTrade") == 0) { accept(); return;}
	if (_stricmp(str,"ExoticBondCashFlows") == 0) { accept(); return;}
	if (_stricmp(str,"ExoticBondTrade") == 0) { accept(); return;}
	if (_stricmp(str,"ExoticHybridTrade") == 0) { accept(); return;}
	if (_stricmp(str,"ExoticlnterestRateTrade") == 0) { accept(); return;}
	if (_stricmp(str,"ExtendedRiskyFixedCashFlows") == 0) { accept(); return;}
	if (_stricmp(str,"ExternalTrade") == 0) { accept(); return;}
	if (_stricmp(str,"FinanceAdjustment") == 0) { accept(); return;}
	if (_stricmp(str,"FirstToDefaultCDS") == 0) { accept(); return;}
	if (_stricmp(str,"FirstToDefaultCDSBasket") == 0) { accept(); return;}
	if (_stricmp(str,"FixedAssetSwap") == 0) { accept(); return;}
	if (_stricmp(str,"FixedCashFlows") == 0) { accept(); return;}
	if (_stricmp(str,"FixedLeg") == 0) { accept(); return;}
	if (_stricmp(str,"FixedLegPartiallyCompounded") == 0) { accept(); return;}
	if (_stricmp(str,"FixedLegUntilDefault") == 0) { accept(); return;}
	if (_stricmp(str,"FloatingLegUntilDefault") == 0) { accept(); return;}
	if (_stricmp(str,"FloatingLegVithRatio") == 0) { accept(); return;}
	if (_stricmp(str,"FloatingSinglePaymentOnDefault") == 0) { accept(); return;}
	if (_stricmp(str,"FloatingTermDeposit") == 0) { accept(); return;}
	if (_stricmp(str,"ForwardBondTrade") == 0) { accept(); return;}
	if (_stricmp(str,"ForwardRateAgreement") == 0) { accept(); return;}
	if (_stricmp(str,"ForwardStartSwaption") == 0) { accept(); return;}
	if (_stricmp(str,"FRNOption") == 0) { accept(); return;}
	if (_stricmp(str,"FRNTrade") == 0) { accept(); return;}
	if (_stricmp(str,"FundingPortfolio") == 0) { accept(); return;}
	if (_stricmp(str,"FxBarrier2") == 0) { accept(); return;}
	if (_stricmp(str,"FxBermudan") == 0) { accept(); return;}
	if (_stricmp(str,"FxBermudanQuanto") == 0) { accept(); return;}
	if (_stricmp(str,"FXCMSDualRateOption") == 0) { accept(); return;}
	if (_stricmp(str,"FxDigital") == 0) { accept(); return;}
	if (_stricmp(str,"FXForward") == 0) { accept(); return;}
	if (_stricmp(str,"FXForwardBasket") == 0) { accept(); return;}
	if (_stricmp(str,"FxFuture") == 0) { accept(); return;}
	if (_stricmp(str,"FXGenericTrade") == 0) { accept(); return;}
	if (_stricmp(str,"FxMultipleOption") == 0) { accept(); return;}
	if (_stricmp(str,"FxOption") == 0) { accept(); return;}
	if (_stricmp(str,"FxOptionLeg") == 0) { accept(); return;}
	if (_stricmp(str,"FxOptionStrip") == 0) { accept(); return;}
	if (_stricmp(str,"FxSwapRolling") == 0) { accept(); return;}
	if (_stricmp(str,"FxTARN") == 0) { accept(); return;}
	if (_stricmp(str,"FxVarianceSwap") == 0) { accept(); return;}
	if (_stricmp(str,"GearedCallableRF") == 0) { accept(); return;}
	if (_stricmp(str,"GeneralCallableTrade") == 0) { accept(); return;}
	if (_stricmp(str,"GenericFuture") == 0) { accept(); return;}
	if (_stricmp(str,"GenericFutureOption") == 0) { accept(); return;}
	if (_stricmp(str,"HighGradeDCBTrade") == 0) { accept(); return;}
	if (_stricmp(str,"HighYieldBondOption") == 0) { accept(); return;}
	if (_stricmp(str,"HighYieldBondTrade") == 0) { accept(); return;}
	if (_stricmp(str,"HighYieldDCBTrade") == 0) { accept(); return;}
	if (_stricmp(str,"InflationAssetSwap") == 0) { accept(); return;}
	if (_stricmp(str,"InflationCapFloor") == 0) { accept(); return;}
	if (_stricmp(str,"InflationCapFloorZeroCoupon") == 0) { accept(); return;}
	if (_stricmp(str,"InflationCashFlows") == 0) { accept(); return;}
	if (_stricmp(str,"InflationCMSSpread") == 0) { accept(); return;}
	if (_stricmp(str,"InflationCrossCurrencySwap") == 0) { accept(); return;}
	if (_stricmp(str,"InflationExoticBondAssetSwap") == 0) { accept(); return;}
	if (_stricmp(str,"InflationIndexFloatingSwap") == 0) { accept(); return;}
	if (_stricmp(str,"InflationlndexForward") == 0) { accept(); return;}
	if (_stricmp(str,"InflationlndexFuture") == 0) { accept(); return;}
	if (_stricmp(str,"InflationLeg") == 0) { accept(); return;}
	if (_stricmp(str,"InflationLPlFlows") == 0) { accept(); return;}
	if (_stricmp(str,"InflationLPISwap") == 0) { accept(); return;}
	if (_stricmp(str,"InflationPaymentOnDefault") == 0) { accept(); return;}
	if (_stricmp(str,"InflationQuantoCapFloor") == 0) { accept(); return;}
	if (_stricmp(str,"InflationQuantoLeg") == 0) { accept(); return;}
	if (_stricmp(str,"InflationSpreadOptionTrade") == 0) { accept(); return;}
	if (_stricmp(str,"InflationSwapleroCoupon") == 0) { accept(); return;}
	if (_stricmp(str,"InterestOnlnterest") == 0) { accept(); return;}
	if (_stricmp(str,"InterestRateBundleFuture") == 0) { accept(); return;}
	if (_stricmp(str,"InterestRateBundleFutureOption") == 0) { accept(); return;}
	if (_stricmp(str,"InterestRateFuture") == 0) { accept(); return;}
	if (_stricmp(str,"InterestRateFutureOption") == 0) { accept(); return;}
	if (_stricmp(str,"InterestRateFutureOptionOTC") == 0) { accept(); return;}
	if (_stricmp(str,"InterestRateSwap") == 0) { accept(); return;}
	if (_stricmp(str,"InterestRateSwapCashSettle") == 0) { accept(); return;}
	if (_stricmp(str,"InterestRateSwapFuture") == 0) { accept(); return;}
	if (_stricmp(str,"IrregularMultiCurrencySwaption") == 0) { accept(); return;}
	if (_stricmp(str,"IrregularSwaption") == 0) { accept(); return;}
	if (_stricmp(str,"LeveragedCDSBasketTranche") == 0) { accept(); return;}
	if (_stricmp(str,"LeveragedCDSGap") == 0) { accept(); return;}
	if (_stricmp(str,"LeveragedSinglePaymentOnDefault") == 0) { accept(); return;}
	if (_stricmp(str,"LMMExoticSwap") == 0) { accept(); return;}
	if (_stricmp(str,"ManagedPortfolio") == 0) { accept(); return;}
	if (_stricmp(str,"MidCurveFutureOption") == 0) { accept(); return;}
	if (_stricmp(str,"MortgageOption") == 0) { accept(); return;}
	if (_stricmp(str,"MortgageTBATrade") == 0) { accept(); return;}
	if (_stricmp(str,"NonDeliverableFXForward") == 0) { accept(); return;}
	if (_stricmp(str,"NonDeliverableFxLeg") == 0) { accept(); return;}
	if (_stricmp(str,"NonDeliverableTrade") == 0) { accept(); return;}
	if (_stricmp(str,"NonLiborlRFuture") == 0) { accept(); return;}
	if (_stricmp(str,"NonLiborlRFutureOption") == 0) { accept(); return;}
	if (_stricmp(str,"NonStandardIndexSwaption") == 0) { accept(); return;}
	if (_stricmp(str,"OnshorelRFuture") == 0) { accept(); return;}
	if (_stricmp(str,"PeriodicCapFloor") == 0) { accept(); return;}
	if (_stricmp(str,"PortfolioCashFlows") == 0) { accept(); return;}
	if (_stricmp(str,"PRD") == 0) { accept(); return;}
	if (_stricmp(str,"ProjectFinanceTrade") == 0) { accept(); return;}
	if (_stricmp(str,"QCMSCapFloor") == 0) { accept(); return;}
	if (_stricmp(str,"QCMSFxDigitalLeg") == 0) { accept(); return;}
	if (_stricmp(str,"QCMSFxLeg") == 0) { accept(); return;}
	if (_stricmp(str,"QCMSLeg") == 0) { accept(); return;}
	if (_stricmp(str,"QuantoCDS") == 0) { accept(); return;}
	if (_stricmp(str,"QuantoFxForward") == 0) { accept(); return;}
	if (_stricmp(str,"QuantoFxOption") == 0) { accept(); return;}
	if (_stricmp(str,"RateDigital") == 0) { accept(); return;}
	if (_stricmp(str,"RateDigitalLeg") == 0) { accept(); return;}
	if (_stricmp(str,"RateDigitalQCMSLeg") == 0) { accept(); return;}
	if (_stricmp(str,"Ref") == 0) { accept(); return;}
	if (_stricmp(str,"lexTrade") == 0) { accept(); return;}
	if (_stricmp(str,"RegularReflexTrade") == 0) { accept(); return;}
	if (_stricmp(str,"ResettableCapFloor") == 0) { accept(); return;}
	if (_stricmp(str,"RiskyAssetSwap") == 0) { accept(); return;}
	if (_stricmp(str,"RiskyBermudanSwaption") == 0) { accept(); return;}
	if (_stricmp(str,"RiskyCrossCurrencySwap") == 0) { accept(); return;}
	if (_stricmp(str,"RiskyFixedCashFlows") == 0) { accept(); return;}
	if (_stricmp(str,"RiskyFRNTrade") == 0) { accept(); return;}
	if (_stricmp(str,"RiskyInterestRateTrade") == 0) { accept(); return;}
	if (_stricmp(str,"RollingGuaranteeLeg") == 0) { accept(); return;}
	if (_stricmp(str,"SemiAsianResettable") == 0) { accept(); return;}
	if (_stricmp(str,"SinglePaymentOnDefault") == 0) { accept(); return;}
	if (_stricmp(str,"Snowball") == 0) { accept(); return;}
	if (_stricmp(str,"SpreadOptionEquityTrade") == 0) { accept(); return;}
	if (_stricmp(str,"SpreadOptionTrade") == 0) { accept(); return;}
	if (_stricmp(str,"StructuredLoan") == 0) { accept(); return;}
	if (_stricmp(str,"SwapFloatingLeg") == 0) { accept(); return;}
	if (_stricmp(str,"SwapFloatingLegMtM") == 0) { accept(); return;}
	if (_stricmp(str,"SwapFloatingLegXCcyRate") == 0) { accept(); return;}
	if (_stricmp(str,"Swaption") == 0) { accept(); return;}
	if (_stricmp(str,"SwaptionOnDefault") == 0) { accept(); return;}
	if (_stricmp(str,"SwaptionOnDefaultCrossCurrency") == 0) { accept(); return;}
	if (_stricmp(str,"SwaptionOnDefaultCrossCurrencyFixed") == 0) { accept(); return;}
	if (_stricmp(str,"SwaptionOnDefaultCrossCurrencyFloating") == 0) { accept(); return;}
	if (_stricmp(str,"SyntheticBondFutureOptionTrade") == 0) { accept(); return;}
	if (_stricmp(str,"SyntheticBondFutureTrade") == 0) { accept(); return;}
	if (_stricmp(str,"SyntheticlnterestRateFuture") == 0) { accept(); return;}
	if (_stricmp(str,"SyntheticlnterestRateFutureOption") == 0) { accept(); return;}
	if (_stricmp(str,"SyntheticOnshorelRFuture") == 0) { accept(); return;}
	if (_stricmp(str,"TargetPRD") == 0) { accept(); return;}
	if (_stricmp(str,"TargetSpreadOption") == 0) { accept(); return;}
	if (_stricmp(str,"TotalReturnSwapCustomCSA") == 0) { accept(); return;}
	if (_stricmp(str,"TotalReturnSwapIndex") == 0) { accept(); return;}
	if (_stricmp(str,"TotalReturnSwapLeg") == 0) { accept(); return;}
	if (_stricmp(str,"TriggerCorridor") == 0) { accept(); return;}
	if (_stricmp(str,"TriggerEquityLiborOption") == 0) { accept(); return;}
	if (_stricmp(str,"TriggerEquityRatchet") == 0) { accept(); return;}
	if (_stricmp(str,"TriggerReverseFloater") == 0) { accept(); return;}
	if (_stricmp(str,"TriggerSnowball") == 0) { accept(); return;}
	if (_stricmp(str,"VolatilityBond") == 0) { accept(); return;}
	if (_stricmp(str,"ZCBondCollateralTrade") == 0) { accept(); return;}
}


FUNCPTR dumbass2(char * str, fsa& automata)
//FUNCPTR dumbass2(char * str, int pos)
{
	int hash = hashString(str, strlen(str));
	//int hash = hashtable[pos].hashcode;
	FUNCPTR func = NULL;
	int lastPos = 324;
/*
	for (int i = 0; i < lastPos; ++i)
		{
		if (hash == hashtable[i].hashcode)
		{
			func = hashtable[i].func;
			break;
		}
	}
	if (func) func();
	return func;
*/
// do a binsearch
	int hi, lo;
	lo = 0; hi = lastPos;
	int mid;
	int midval;
	while (lo <= hi)
	{
		mid = (lo + hi) / 2;
		midval = hashtable[mid].hashcode;
		if (midval < hash)
		{
			lo = mid + 1;
		}
		else if (midval > hash)
		{
			hi = mid - 1;
		}
		else
		{
			func = hashtable[mid].func;
			break;
		}
	}
	if (func) func();
	return func;
}



FUNCPTR dumbass3(char * str, fsa& automata)
{
	int hash = hashString(str, strlen(str));
	//int hash = hashtable[0].hashcode;
	FUNCPTR func = mappy[hash];
	if (func) func();
	return func;
}



void test()
{
	fsa fsa(256);

	hftimer timer;

	std::ifstream vfs;
	vfs.open("vflist.txt");
	char vfname[128];
	char* vfnames[325];
	int i = 0, j;
	while (!vfs.eof())
	{
		vfs.getline(vfname, 128);
		if (vfs.gcount() > 128) throw "line is too long";
		fsa.addString(vfname, &accept);
		vfnames[i] = new char[strlen(vfname)+1];
		strcpy(vfnames[i], vfname);
		++i;
	}
	vfs.close();
	timer.startTimer();
	for (i = 0; i <= 1000000; ++i)
	{
		dumbass(vfnames[i % 324], fsa);
	}
	std::cout << "Linear string search: "<<timer.getTime() << std::endl;

	// prepare hashcode for dumbass2
	for (i = 0; i < 324; ++i)
	{
		hashtable[i].vfname = vfnames[i];
		hashtable[i].func = &accept;
		hashtable[i].hashcode = hashString(hashtable[i].vfname, strlen(hashtable[i].vfname));
		mappy[hashtable[i].hashcode] = &accept;
	}
	// bubble sort for hash codes
	bool swapped;
	VFNamesWithHashCode aux;
	for (i = 0; i < 324; ++i)
	{
		swapped = false;
		for (j = 1; j < 324-i; ++j)
		{
			if (hashtable[j].hashcode < hashtable[j - 1].hashcode)
			{ // swap
				swapped = true;
				aux = hashtable[j];
				hashtable[j] = hashtable[j - 1];
				hashtable[j - 1] = aux;
			}
		}
		if (!swapped) break;
	}

	timer.startTimer();
	for (i = 0; i <= 1000000; ++i)
	{
		//dumbass2(vfnames[i % 324], fsa);
		dumbass2(vfnames[1], fsa);
	}
	std::cout <<"Bisection search of hashcodes: " <<timer.getTime() << std::endl;


	timer.startTimer();
	for (i = 0; i <= 1000000; ++i)
	{
		//dumbass3(vfnames[i % 324], fsa);
		dumbass3(vfnames[1], fsa);
	}
	std::cout << "map<int, func> search of hashcodes: " << timer.getTime() << std::endl;


	fsa.alternative1();
	timer.startTimer();
	for (i = 0; i <= 1000000; ++i)
	{
		//smartass(vfnames[i % 324], fsa);
		smartass(vfnames[1], fsa);
	}
	std::cout << "FSA alternative 1: " << timer.getTime() << std::endl;
	timer.startTimer();
}