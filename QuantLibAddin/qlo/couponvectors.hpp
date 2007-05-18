
/*
 Copyright (C) 2006, 2007 Ferdinando Ametrano
 Copyright (C) 2005 Aurelien Chanudet
 Copyright (C) 2006, 2007 Cristina Duminuco
 Copyright (C) 2006 Giorgio Facchinetti

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it under the
 terms of the QuantLib license.  You should have received a copy of the
 license along with this program; if not, please email quantlib-dev@lists.sf.net
 The license is also available online at http://quantlib.org/html/license.html

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

#ifndef qla_couponvectors_hpp
#define qla_couponvectors_hpp

#include <oh/objecthandler.hpp>

#include <qlo/schedule.hpp>
#include <qlo/index.hpp>
#include <qlo/analysis.hpp>
#include <ql/termstructures/volatilities/all.hpp>

#include <ql/cashflows/digitalcoupon.hpp>

namespace QuantLibAddin {
    
    class Leg : public ObjectHandler::Object {
      public:
        QuantLib::Rate lastCouponRate(const QuantLib::Date& refDate) const;
        QuantLib::Rate currentCouponRate(const QuantLib::Date& refDate) const;
        QuantLib::Date startDate() const;
        QuantLib::Date maturityDate() const;
        QuantLib::Real npv(const QuantLib::Handle<QuantLib::YieldTermStructure>&) const;
        QuantLib::Real bps(const QuantLib::Handle<QuantLib::YieldTermStructure>&) const;
        QuantLib::Rate atmRate(const QuantLib::Handle<QuantLib::YieldTermStructure>&) const;
        void setPricer(const boost::shared_ptr<QuantLib::FloatingRateCouponPricer>& pricer);
        void setPricers(const std::vector<boost::shared_ptr<QuantLib::FloatingRateCouponPricer> >& pricers);

        std::vector<std::vector<boost::any> > analysis() const;
        const QuantLib::Leg& getQuantLibLeg();        
      protected:
        // copy or shared_ptr?
        QuantLib::Leg leg_;
    };

    class MultiPhaseLeg : public Leg {
      public:
        MultiPhaseLeg(const std::vector<boost::shared_ptr<Leg> >& legs,
                      bool toBeSorted);
    };

    class SimpleCashFlowVector : public Leg {
      public:
        SimpleCashFlowVector(const std::vector<QuantLib::Real>& amounts, 
                             const std::vector<QuantLib::Date>& dates);
    };
    
    class FixedRateLeg : public Leg {
      public:
        FixedRateLeg(
            QuantLib::BusinessDayConvention              paymentConvention,
            const std::vector<QuantLib::Real>&           nominals,
            const boost::shared_ptr<QuantLib::Schedule>& schedule,
            const std::vector<QuantLib::Rate>&           couponRates,
            const QuantLib::DayCounter&                  paymentDayCounter);
    };

    class IborLeg : public Leg {
      public:
        IborLeg(
            QuantLib::BusinessDayConvention paymentConvention,
            const std::vector<QuantLib::Real>& nominals,
            const boost::shared_ptr<QuantLib::Schedule>& schedule,
            QuantLib::Natural fixingDays,
            bool isInArrears,
            const QuantLib::DayCounter& paymentDayCounter,
            const std::vector<QuantLib::Rate>& floors,
            const std::vector<QuantLib::Real>& gearings,
            const boost::shared_ptr<QuantLib::IborIndex>& index,
            const std::vector<QuantLib::Spread>& spreads,
            const std::vector<QuantLib::Rate>& caps);
    };

    class DigitalIborLeg : public Leg {
      public:
        DigitalIborLeg(
            QuantLib::BusinessDayConvention paymentConvention,
            const std::vector<QuantLib::Real>& nominals,
            const boost::shared_ptr<QuantLib::Schedule>& schedule,
            QuantLib::Natural fixingDays,
            bool isInArrears,
            const QuantLib::DayCounter& paymentDayCounter,
            const std::vector<QuantLib::Real>& gearings,
            const boost::shared_ptr<QuantLib::IborIndex>& index,
            const std::vector<QuantLib::Spread>& spreads,
            const std::vector<QuantLib::Rate>& callRates,
            bool isCallOptionAdded,
            const std::vector<QuantLib::Rate>& putRates,
            bool isPutOptionAdded,
            const std::vector<QuantLib::Rate>& digitalPayoffs,
            QuantLib::Replication::Type replicationType,
            QuantLib::Real eps);
    };

    class CmsLeg : public Leg {
      public:
        CmsLeg(
            QuantLib::BusinessDayConvention paymentConvention,
            const std::vector<QuantLib::Real>& nominals,
            const boost::shared_ptr<QuantLib::Schedule>& schedule,
            QuantLib::Natural fixingDays,
            bool isInArrears,
            const QuantLib::DayCounter& paymentDayCounter,
            const std::vector<QuantLib::Rate>& floors,
            const std::vector<QuantLib::Real>& gearings,
            const boost::shared_ptr<QuantLib::SwapIndex>& index,
            const std::vector<QuantLib::Spread>& spreads,
            const std::vector<QuantLib::Rate>& caps);
    };

    class DigitalCmsLeg : public Leg {
      public:
        DigitalCmsLeg(
            QuantLib::BusinessDayConvention paymentConvention,
            const std::vector<QuantLib::Real>& nominals,
            const boost::shared_ptr<QuantLib::Schedule>& schedule,
            QuantLib::Natural fixingDays,
            bool isInArrears,
            const QuantLib::DayCounter& paymentDayCounter,
            const std::vector<QuantLib::Real>& gearings,
            const boost::shared_ptr<QuantLib::SwapIndex>& index,
            const std::vector<QuantLib::Spread>& spreads,
            const std::vector<QuantLib::Rate>& callRates,
            bool isCallOptionAdded,
            const std::vector<QuantLib::Rate>& putRates,
            bool isPutOptionAdded,
            const std::vector<QuantLib::Rate>& digitalPayoffs,
            QuantLib::Replication::Type replicationType,
            QuantLib::Real eps);
    };

    class RangeAccrualLeg : public Leg {
      public:
        RangeAccrualLeg(
           QuantLib::BusinessDayConvention paymentConvention,
           const std::vector<QuantLib::Real>& nominals,
           const boost::shared_ptr<QuantLib::Schedule>& schedule,
           QuantLib::Natural fixingDays,
           const QuantLib::DayCounter& paymentDayCounter,
           const std::vector<QuantLib::Rate>& lowerTriggers,
           const std::vector<QuantLib::Real>& gearings,
           const boost::shared_ptr<QuantLib::IborIndex>& index,
           const std::vector<QuantLib::Spread>& spreads,
           const std::vector<QuantLib::Rate>& upperTriggers,
           const QuantLib::Period& observationTenor,
           QuantLib::BusinessDayConvention observationConvention);
    };

    class CmsZeroLeg : public Leg {
      public:
        CmsZeroLeg(
            QuantLib::BusinessDayConvention paymentConvention,
            const std::vector<QuantLib::Real>& nominals,
            const boost::shared_ptr<QuantLib::Schedule>& schedule,
            QuantLib::Natural fixingDays,
            bool isInArrears,
            const QuantLib::DayCounter& paymentDayCounter,
            const std::vector<QuantLib::Rate>& floors,
            const std::vector<QuantLib::Real>& gearings,
            const boost::shared_ptr<QuantLib::SwapIndex>& index,
            const std::vector<QuantLib::Spread>& spreads,
            const std::vector<QuantLib::Rate>& caps);
    };

    class FloatingRateCouponPricer : 
        public ObjectHandler::LibraryObject<QuantLib::FloatingRateCouponPricer> {
      public:
          FloatingRateCouponPricer(){};
    };
    
    class IborCouponPricer : public FloatingRateCouponPricer {
      public:
        IborCouponPricer(
            const QuantLib::Handle<QuantLib::CapletVolatilityStructure>& vol,
            const std::string& typeOfIborCouponPricer);
    };
}

#endif
