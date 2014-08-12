/**
 * Copyright 2014 MongoDB Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "mongoc.h"

#include "bson/document.hpp"

#include "driver/base/client.hpp"
#include "driver/base/collection.hpp"
#include "driver/base/cursor.hpp"
#include "driver/base/database.hpp"
#include "driver/base/options.hpp"

#include "driver/model/find.hpp"
#include "driver/model/find_one_and_remove.hpp"
#include "driver/model/find_one_and_replace.hpp"
#include "driver/model/find_one_and_update.hpp"
#include "driver/model/replace.hpp"
#include "driver/model/update.hpp"

#include "driver/result/distinct.hpp"
#include "driver/result/explain.hpp"
#include "driver/result/write.hpp"
