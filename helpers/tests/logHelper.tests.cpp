#include "tests/KengineTest.hpp"
#include "helpers/logHelper.hpp"

struct logHelper : KengineTest {};

TEST_F(logHelper, logHelper) {
    std::vector<kengine::LogEvent> output;

    kengine::entities += [&](kengine::Entity & e) {
        e += kengine::functions::Log{
            [&](const kengine::LogEvent & log) {
                output.push_back(log);
            }
        };
    };

    kengine_log(Verbose, "Category", "Message");
    kengine_logf(Warning, "OtherCategory", "%s", "OtherMessage");

    EXPECT_EQ(output.size(), 2);

    EXPECT_EQ(output[0].severity, kengine::LogSeverity::Verbose);
    EXPECT_STREQ(output[0].category, "Category");
    EXPECT_STREQ(output[0].message, "Message");

    EXPECT_EQ(output[1].severity, kengine::LogSeverity::Warning);
    EXPECT_STREQ(output[1].category, "OtherCategory");
    EXPECT_STREQ(output[1].message, "OtherMessage");
}