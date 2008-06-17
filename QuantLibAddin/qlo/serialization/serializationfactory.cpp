
/*  
 Copyright (C) 2007, 2008 Eric Ehlers
 Copyright (C) 2006 Plamen Neykov
 
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

#include <qlo/serialization/serializationfactory.hpp>
#include <qlo/serialization/create/create_all.hpp>

#include <oh/ohdefines.hpp>
#include <fstream>
#include <set>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/vector.hpp>
#include <qlo/serialization/register/serialization_register.hpp>

namespace QuantLibAddin {

    SerializationFactory::SerializationFactory() {    

        registerCreators();

    }

    void SerializationFactory::register_out(boost::archive::xml_oarchive &ar,
        std::vector<boost::shared_ptr<ObjectHandler::ValueObject> >& valueObjects){

            tpl_register_classes(ar);
            ar << boost::serialization::make_nvp("object_list", valueObjects);
    }


    void SerializationFactory::register_in(boost::archive::xml_iarchive &ar,
        std::vector<boost::shared_ptr<ObjectHandler::ValueObject> >& valueObjects){

            tpl_register_classes(ar);
            ar >> boost::serialization::make_nvp("object_list", valueObjects);
    }


}

