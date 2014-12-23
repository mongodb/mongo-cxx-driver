#include <iostream>
#include <sstream>
#include <cstdlib>
#include <chrono>

#include <bson.h>
#include "bson/builder.hpp"

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "usage - " << argv[0] << " ITERATIONS [TESTS]" << std::endl;
        return 1;
    }

    long iterations = atol(argv[1]);
    long tests = 255;

    if (argc == 3) {
        tests = atol(argv[2]);
    }

    if (tests & 1) {
        auto before = std::chrono::steady_clock::now();

        bson::builder::document b;

        for (long i = 0; i < iterations; i++) {
            using namespace bson::builder::helpers;

            b.clear();

#include "bson/builder/macros_on.hpp"
            b << "level1"
              << DOC("level2" << DOC("level3" << ARRAY(ARRAY(ARRAY(1 << 2 << 3 << 4 << 5 << "hello"
                                                                     << "world")))));
#include "bson/builder/macros_off.hpp"
        }

        auto after = std::chrono::steady_clock::now();

        std::cout << "builder: iterations = " << iterations << ", duration = "
                  << std::chrono::duration_cast<std::chrono::milliseconds>(after - before).count()
                  << std::endl;
    }

    if (tests & 2) {
        auto before = std::chrono::steady_clock::now();

        bson_t bson;
        bson_init(&bson);

        for (long i = 0; i < iterations; i++) {
            bson_reinit(&bson);

            BCON_APPEND(&bson, "level1", "{", "level2", "{", "level3", "[", "[", "[", BCON_INT32(1),
                        BCON_INT32(2), BCON_INT32(3), BCON_INT32(4), BCON_INT32(5), "hello",
                        "world", "]", "]", "]", "}", "}");
        }

        auto after = std::chrono::steady_clock::now();

        std::cout << "bcon: iterations = " << iterations << ", duration = "
                  << std::chrono::duration_cast<std::chrono::milliseconds>(after - before).count()
                  << std::endl;
    }

    if (tests & 4) {
        auto before = std::chrono::steady_clock::now();

        bson_t bson;
        bson_init(&bson);

        for (long i = 0; i < iterations; i++) {
            bson_reinit(&bson);

            bson_t level1, level2, level3, array1, array2, array3;

            bson_append_document_begin(&bson, "level1", 6, &level1);
            bson_append_document_begin(&level1, "level2", 6, &level2);
            bson_append_document_begin(&level2, "level3", 6, &level3);
            bson_append_document_begin(&level3, "array1", 6, &array1);
            bson_append_document_begin(&array1, "array2", 6, &array2);
            bson_append_document_begin(&array2, "array3", 6, &array3);
            bson_append_int32(&array3, "0", 1, 1);
            bson_append_int32(&array3, "1", 1, 2);
            bson_append_int32(&array3, "2", 1, 3);
            bson_append_int32(&array3, "3", 1, 4);
            bson_append_int32(&array3, "4", 1, 5);
            bson_append_document_end(&array2, &array3);
            bson_append_document_end(&array1, &array2);
            bson_append_document_end(&level3, &array1);
            bson_append_document_end(&level2, &level3);
            bson_append_document_end(&level1, &level2);
            bson_append_document_end(&bson, &level1);
        }

        auto after = std::chrono::steady_clock::now();

        std::cout << "bson: iterations = " << iterations << ", duration = "
                  << std::chrono::duration_cast<std::chrono::milliseconds>(after - before).count()
                  << std::endl;
    }

    if (tests & 8) {
        auto before = std::chrono::steady_clock::now();

        bson::builder::document b;

        for (long i = 0; i < iterations; i++) {
            using namespace bson::builder::helpers;

            b.clear();

            b << "a" << 1 << "b" << 2 << "c"
              << "d";
        }

        auto after = std::chrono::steady_clock::now();

        std::cout << "builder-simple: iterations = " << iterations << ", duration = "
                  << std::chrono::duration_cast<std::chrono::milliseconds>(after - before).count()
                  << std::endl;
    }

    if (tests & 16) {
        auto before = std::chrono::steady_clock::now();

        bson_t bson;
        bson_init(&bson);

        for (long i = 0; i < iterations; i++) {
            bson_reinit(&bson);

            BCON_APPEND(&bson, "a", BCON_INT32(1), "b", BCON_INT32(2), "c", "d");
        }

        auto after = std::chrono::steady_clock::now();

        std::cout << "bcon-simple: iterations = " << iterations << ", duration = "
                  << std::chrono::duration_cast<std::chrono::milliseconds>(after - before).count()
                  << std::endl;
    }

    if (tests & 32) {
        auto before = std::chrono::steady_clock::now();

        bson_t bson;
        bson_init(&bson);

        for (long i = 0; i < iterations; i++) {
            bson_reinit(&bson);

            bson_append_int32(&bson, "a", 1, 1);
            bson_append_int32(&bson, "b", 1, 2);
            bson_append_utf8(&bson, "c", 1, "d", 1);
        }

        auto after = std::chrono::steady_clock::now();

        std::cout << "bson-simple: iterations = " << iterations << ", duration = "
                  << std::chrono::duration_cast<std::chrono::milliseconds>(after - before).count()
                  << std::endl;
    }

    if (tests & 64) {
        using namespace bson::builder::helpers;

        auto before = std::chrono::steady_clock::now();

        bson::builder::document b;

#include "bson/builder/macros_on.hpp"
        b << "level1"
          << DOC("level2" << DOC("level3" << ARRAY(ARRAY(ARRAY(1 << 2 << 3 << 4 << 5 << "hello"
                                                                 << "world")))));
#include "bson/builder/macros_off.hpp"

        for (long i = 0; i < iterations; i++) {
            std::stringstream s;

            s << b.view();
        }

        auto after = std::chrono::steady_clock::now();

        std::cout << "builder-complex dump: iterations = " << iterations << ", duration = "
                  << std::chrono::duration_cast<std::chrono::milliseconds>(after - before).count()
                  << std::endl;
    }

    if (tests & 128) {
        using namespace bson::builder::helpers;

        auto before = std::chrono::steady_clock::now();

        bson::builder::document b;

#include "bson/builder/macros_on.hpp"
        b << "level1"
          << DOC("level2" << DOC("level3" << ARRAY(ARRAY(ARRAY(1 << 2 << 3 << 4 << 5 << "hello"
                                                                 << "world")))));
#include "bson/builder/macros_off.hpp"

        bson_t bson;
        bson_init_static(&bson, b.view().get_buf(), b.view().get_len());

        for (long i = 0; i < iterations; i++) {
            char* json = bson_as_json(&bson, NULL);
            free(json);
        }

        auto after = std::chrono::steady_clock::now();

        std::cout << "bson-complex dump: iterations = " << iterations << ", duration = "
                  << std::chrono::duration_cast<std::chrono::milliseconds>(after - before).count()
                  << std::endl;
    }

    return 0;
}
