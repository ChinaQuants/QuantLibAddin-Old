
/*
 Copyright (C) 2005 Ferdinando Ametrano
 Copyright (C) 2004, 2005, 2006 Eric Ehlers

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

#if defined(HAVE_CONFIG_H)     // Dynamically created by configure
    #include <oh/config.hpp>
#endif
#include <oh/objecthandlerbase.hpp>
#include <oh/exception.hpp>
#include <ostream>
#include <sstream>
#include <boost/regex.hpp>

namespace ObjHandler {

    std::string ObjectHandlerBase::storeObject(const std::string &instanceName, 
            const obj_ptr &object) {
        object->setInstanceName(boost::shared_ptr < Object::InstanceName > (new Object::InstanceName(instanceName)));
        objectList_[instanceName] = object;
        return instanceName;
    }

    obj_ptr ObjectHandlerBase::retrieveObject(const std::string &fullName) const {
        ObjectList::const_iterator result = objectList_.find(fullName);
        if (result == objectList_.end()) {
            std::ostringstream msg;
            msg << "ObjectHandler error: attempt to retrieve object "
                << "with unknown instance name '" << fullName << "'";
            throw Exception(msg.str());
        } else
            return result->second;
    }

    void ObjectHandlerBase::deleteObject(const std::string &fullName) {
        objectList_.erase(fullName);
    }

    void ObjectHandlerBase::deleteAllObjects() {
        objectList_.clear();
    }

    void ObjectHandlerBase::dump(std::ostream& out) {
        out << "dump of all objects in ObjectHandler:" << std::endl;
        for (ObjectList::const_iterator i=objectList_.begin();
            i!=objectList_.end(); i++) {
            obj_ptr object = i->second;
            out << "Object with handle = " << i->first << ":" << std::endl << *object.get();
        }
    }

    const int ObjectHandlerBase::objectCount() {
        return objectList_.size();
    }

    const std::vector < std::string > ObjectHandlerBase::listInstanceNames(const std::string regex) {
        std::vector < std::string > instanceNames;
        if (regex.empty()) {
            for (ObjectList::const_iterator i=objectList_.begin(); i!=objectList_.end(); i++)
                instanceNames.push_back(i->first);
        } else {
            boost::regex r(regex);
            for (ObjectList::const_iterator i=objectList_.begin(); i!=objectList_.end(); i++)
                if (regex_match(i->first, r)) instanceNames.push_back(i->first);
        }
        return instanceNames;
    }
}

