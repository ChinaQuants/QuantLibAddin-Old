/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2006, 2007 Ferdinando Ametrano
 Copyright (C) 2006 Katiuscia Manzoni
 Copyright (C) 2005 Eric Ehlers
 Copyright (C) 2005 Plamen Neykov

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/license.shtml>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

#if defined(HAVE_CONFIG_H)
    #include <qlo/config.hpp>
#endif

#include <qlo/index.hpp>

#include <ql/indexes/bmaindex.hpp>

#include <ql/indexes/ibor/euribor.hpp>
#include <ql/indexes/ibor/eurlibor.hpp>

#include <ql/indexes/swap/euriborswapfixa.hpp>
#include <ql/indexes/swap/euriborswapfixb.hpp>
#include <ql/indexes/swap/euriborswapfixifr.hpp>

#include <ql/indexes/swap/eurliborswapfixa.hpp>
#include <ql/indexes/swap/eurliborswapfixb.hpp>
#include <ql/indexes/swap/eurliborswapfixifr.hpp>

#include <ql/utilities/dataparsers.hpp>

#include <boost/algorithm/string/case_conv.hpp>

using boost::algorithm::to_upper_copy;

namespace QuantLibAddin {

    void Index::addFixings(const std::vector<QuantLib::Date>& dates,
                           const std::vector<QuantLib::Real>& values,
                           bool forceOverwrite, bool updateValuObject) {
        QL_REQUIRE(dates.size()==values.size(),
                   "size mismatch between dates (" << dates.size() <<
                   ") and values (" << values.size() << ")");
        std::vector<QuantLib::Date> d;
        std::vector<QuantLib::Real> v;
        for (QuantLib::Size i=0; i<values.size(); ++i) {
            // skip null fixings
            if (values[i]!=0.0 && values[i]!=QuantLib::Null<QuantLib::Real>()) {
                QL_REQUIRE(values[i]>0.0,
                           "non positive fixing (" << values[i] <<
                           ") at date " << dates[i] << " not allowed");
                d.push_back(dates[i]);
                v.push_back(values[i]);
            }
        }
        libraryObject_->addFixings(d.begin(), d.end(),
                                   v.begin(), forceOverwrite);

        if(updateValuObject) {
            std::vector<long> fixingDates;
            std::vector<QuantLib::Real> fixingRates;
            const QuantLib::TimeSeries<QuantLib::Real>& history = libraryObject_->timeSeries();
            for(QuantLib::TimeSeries<QuantLib::Real>::const_iterator hi = history.begin(); hi != history.end(); ++hi) {
                fixingDates.push_back(hi->first.serialNumber());
                fixingRates.push_back(hi->second);
            }
            boost::shared_ptr<ObjectHandler::ValueObject> inst_properties = properties();
            inst_properties->setProperty("IndexFixingDates", fixingDates);
            inst_properties->setProperty("IndexFixingRates", fixingRates);
        }
    }

    IborIndex::IborIndex(
                 const boost::shared_ptr<ObjectHandler::ValueObject>& properties,
                 const std::string& indexName,
                 const QuantLib::Period& p,
                 const QuantLib::Natural fixingDays,
                 const QuantLib::Currency& crr,
                 const QuantLib::Calendar& calendar,
                 QuantLib::BusinessDayConvention fltBDC,
                 bool endOfMonth,
                 const QuantLib::DayCounter& fltDayCounter,
                 const QuantLib::Handle<QuantLib::YieldTermStructure>& hYTS,
                 bool permanent) : InterestRateIndex(properties, permanent)
    {
        libraryObject_ = boost::shared_ptr<QuantLib::IborIndex>(new
            QuantLib::IborIndex(indexName, 
                                p,
                                fixingDays, crr, calendar, 
                                fltBDC, endOfMonth, fltDayCounter,
                                hYTS));
    }

    Euribor::Euribor(const boost::shared_ptr<ObjectHandler::ValueObject>& properties,
                     const std::string& p,
                     const QuantLib::Handle<QuantLib::YieldTermStructure>& h,
                     bool permanent) : IborIndex(properties, permanent)
    {
        if (to_upper_copy(p)=="ON")
            libraryObject_ = boost::shared_ptr<QuantLib::IborIndex>(new
                QuantLib::DailyTenorEuribor(0, h));
        else if (to_upper_copy(p)=="1D")
            QL_FAIL("1D is ambigous: please specify ON, TN, or SN");
        else if (to_upper_copy(p)=="TN")
            libraryObject_ = boost::shared_ptr<QuantLib::IborIndex>(new
                QuantLib::DailyTenorEuribor(1, h));
        else if (to_upper_copy(p)=="SN")
            libraryObject_ = boost::shared_ptr<QuantLib::IborIndex>(new
                QuantLib::DailyTenorEuribor(2, h));
        else if (to_upper_copy(p)=="SW")
            libraryObject_ = boost::shared_ptr<QuantLib::IborIndex>(new
            QuantLib::Euribor(1*QuantLib::Weeks, h));
        else {
            QuantLib::Period pp = QuantLib::PeriodParser::parse(p);
            pp.normalize();
            libraryObject_ = boost::shared_ptr<QuantLib::IborIndex>(new
                QuantLib::Euribor(pp, h));
        }
    }

    Euribor365::Euribor365(const boost::shared_ptr<ObjectHandler::ValueObject>& properties,
                           const std::string& p,
                           const QuantLib::Handle<QuantLib::YieldTermStructure>& h,
                           bool permanent) : IborIndex(properties, permanent)
    {
        if (to_upper_copy(p)=="ON")
            libraryObject_ = boost::shared_ptr<QuantLib::IborIndex>(new
                QuantLib::DailyTenorEuribor365(0, h));
        else if (to_upper_copy(p)=="1D")
            QL_FAIL("1D is ambigous: please specify ON, TN, or SN");
        else if (to_upper_copy(p)=="TN")
            libraryObject_ = boost::shared_ptr<QuantLib::IborIndex>(new
                QuantLib::DailyTenorEuribor365(1, h));
        else if (to_upper_copy(p)=="SN")
            libraryObject_ = boost::shared_ptr<QuantLib::IborIndex>(new
                QuantLib::DailyTenorEuribor365(2, h));
        else if (to_upper_copy(p)=="SW")
            libraryObject_ = boost::shared_ptr<QuantLib::IborIndex>(new
                QuantLib::Euribor365(1*QuantLib::Weeks, h));
        else {
            QuantLib::Period pp = QuantLib::PeriodParser::parse(p);
            pp.normalize();
            libraryObject_ = boost::shared_ptr<QuantLib::IborIndex>(new
                QuantLib::Euribor365(pp, h));
        }
    }

    EurLibor::EurLibor(const boost::shared_ptr<ObjectHandler::ValueObject>& properties,
                       const std::string& p,
                       const QuantLib::Handle<QuantLib::YieldTermStructure>& h,
                       bool permanent) : IborIndex(properties, permanent)
    {
        if (to_upper_copy(p)=="ON")
            libraryObject_ = boost::shared_ptr<QuantLib::IborIndex>(new
                QuantLib::DailyTenorEURLibor(0, h));
        else if (to_upper_copy(p)=="1D")
            QL_FAIL("1D is ambigous: please specify ON, TN, or SN");
        else if (to_upper_copy(p)=="TN")
            libraryObject_ = boost::shared_ptr<QuantLib::IborIndex>(new
                QuantLib::DailyTenorEURLibor(1, h));
        else if (to_upper_copy(p)=="SN")
            libraryObject_ = boost::shared_ptr<QuantLib::IborIndex>(new
                QuantLib::DailyTenorEURLibor(2, h));
        else if (to_upper_copy(p)=="SW")
            libraryObject_ = boost::shared_ptr<QuantLib::IborIndex>(new
                QuantLib::EURLibor(1*QuantLib::Weeks, h));
        else {
            QuantLib::Period pp = QuantLib::PeriodParser::parse(p);
            pp.normalize();
            libraryObject_ = boost::shared_ptr<QuantLib::IborIndex>(new
                QuantLib::EURLibor(pp, h));
        }
    }

    SwapIndex::SwapIndex(const boost::shared_ptr<ObjectHandler::ValueObject>& properties,
                         const std::string& familyName,
                         const QuantLib::Period& p,
                         QuantLib::Natural fixingDays,
                         QuantLib::Currency& crr,
                         const QuantLib::Calendar& calendar,
                         const QuantLib::Period& fixedLegTenor,
                         QuantLib::BusinessDayConvention fixedLegBDC,
                         const QuantLib::DayCounter& fixedLegDayCounter,
                         const boost::shared_ptr<QuantLib::IborIndex>& index,
                         bool permanent) : InterestRateIndex(properties, permanent)
    {
        libraryObject_ = boost::shared_ptr<QuantLib::SwapIndex>(new
            QuantLib::SwapIndex(familyName, p,
                                fixingDays, crr, calendar, 
                                fixedLegTenor, fixedLegBDC,
                                fixedLegDayCounter, index));
    }

    EuriborSwapFixA::EuriborSwapFixA(const boost::shared_ptr<ObjectHandler::ValueObject>& properties,
                                     const QuantLib::Period& p,
                                     const QuantLib::Handle<QuantLib::YieldTermStructure>& h,
                                     bool permanent) : SwapIndex(properties, permanent)
    {
        libraryObject_ = boost::shared_ptr<QuantLib::EuriborSwapFixA>(new
            QuantLib::EuriborSwapFixA(p, h));
    }

    EuriborSwapFixB::EuriborSwapFixB(const boost::shared_ptr<ObjectHandler::ValueObject>& properties,
                                     const QuantLib::Period& p,
                                     const QuantLib::Handle<QuantLib::YieldTermStructure>& h,
                                     bool permanent) : SwapIndex(properties, permanent)
    {
        libraryObject_ = boost::shared_ptr<QuantLib::EuriborSwapFixB>(new
            QuantLib::EuriborSwapFixB(p, h));
    }

    EurLiborSwapFixA::EurLiborSwapFixA(const boost::shared_ptr<ObjectHandler::ValueObject>& properties,
                                       const QuantLib::Period& p,
                                       const QuantLib::Handle<QuantLib::YieldTermStructure>& h,
                                       bool permanent) : SwapIndex(properties, permanent)
    {
        libraryObject_ = boost::shared_ptr<QuantLib::EurliborSwapFixA>(new
            QuantLib::EurliborSwapFixA(p, h));
    }

    EurLiborSwapFixB::EurLiborSwapFixB(const boost::shared_ptr<ObjectHandler::ValueObject>& properties,
                                       const QuantLib::Period& p,
                                       const QuantLib::Handle<QuantLib::YieldTermStructure>& h,
                                       bool permanent) : SwapIndex(properties, permanent)
    {
        libraryObject_ = boost::shared_ptr<QuantLib::EurliborSwapFixB>(new
            QuantLib::EurliborSwapFixB(p, h));
    }

    EuriborSwapFixIFR::EuriborSwapFixIFR(const boost::shared_ptr<ObjectHandler::ValueObject>& properties,
                                         const QuantLib::Period& p,
                                         const QuantLib::Handle<QuantLib::YieldTermStructure>& h,
                                         bool permanent) : SwapIndex(properties, permanent)
    {
        libraryObject_ = boost::shared_ptr<QuantLib::EuriborSwapFixIFR>(new
            QuantLib::EuriborSwapFixIFR(p, h));
    }

    EurLiborSwapFixIFR::EurLiborSwapFixIFR(const boost::shared_ptr<ObjectHandler::ValueObject>& properties,
                                           const QuantLib::Period& p,
                                           const QuantLib::Handle<QuantLib::YieldTermStructure>& h,
                                           bool permanent) : SwapIndex(properties, permanent)
    {
        libraryObject_ = boost::shared_ptr<QuantLib::EurliborSwapFixIFR>(new
            QuantLib::EurliborSwapFixIFR(p, h));
    }

    BMAIndex::BMAIndex(
                 const boost::shared_ptr<ObjectHandler::ValueObject>& properties,
                 const QuantLib::Handle<QuantLib::YieldTermStructure>& hYTS,
                 bool permanent) : InterestRateIndex(properties, permanent)
    {
        libraryObject_ = boost::shared_ptr<QuantLib::BMAIndex>(new
            QuantLib::BMAIndex(hYTS));
    }

}
