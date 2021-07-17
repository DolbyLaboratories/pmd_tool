/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2021 by Dolby Laboratories,
 *                Copyright (C) 2021 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING

#include "gtest/gtest.h"
#include "CoreModelTest.h"
#include "dlb_adm/include/dlb_adm_api.h"

using namespace DlbAdm;

static inline void CheckStatus(int x)
{
    if (x != DLB_ADM_STATUS_OK)
    {
        throw false;
    }
}

static inline void CheckTrue(bool x)
{
    if (!x)
    {
        throw false;
    }
}

namespace DlbAdmTest
{

    bool CheckNames(dlb_adm_core_model &model, dlb_adm_entity_id id, dlb_adm_data_names &names)
    {
        bool good = false;

        try
        {
            CoreModel &coreModel = model.GetCoreModel();
            const ModelEntity *entity;
            dlb_adm_bool hasName;
            int status;

            good = coreModel.GetEntity(id, &entity);
            CheckTrue(good);
            status = ::dlb_adm_core_model_has_name(&hasName, &names);
            CheckStatus(status);
            good = !(static_cast<dlb_adm_bool>(entity->HasName()) ^ hasName);   // Same number of names (0 or 1)?
            CheckTrue(good);
            good = (entity->GetLabelCount() == names.label_count);              // Same number of labels (0+)?
            CheckTrue(good);

            EntityName entityName;
            std::string dataName;
            std::string dataLang;
            size_t i = 0;

            if (hasName)                    // Are the names the same?
            {
                good = entity->GetName(entityName, i);
                CheckTrue(good);
                dataName = names.names[i];
                dataLang = names.langs[i];
                good = ((entityName.GetName() == dataName) && (entityName.GetLanguage() == dataLang));
                CheckTrue(good);
                ++i;
            }

            while (i < names.name_count)    // Are the labels the same?
            {
                good = entity->GetName(entityName, i);
                CheckTrue(good);
                dataName = names.names[i];
                dataLang = names.langs[i];
                good = ((entityName.GetName() == dataName) && (entityName.GetLanguage() == dataLang));
                CheckTrue(good);
                ++i;
            }
        }
        catch (bool x)
        {
            good = x;
        }
        catch (...)
        {
            good = false;
        }

        return good;
    }

}

using namespace DlbAdmTest;


class MockEntity : public ModelEntity
{
public:
    explicit MockEntity(dlb_adm_entity_id entityID);

    virtual bool AddLabel(const char *name, const char *language = "");

    virtual bool AddLabel(const std::string &name, const std::string &language);
};

static const size_t MOCK_NAME_LIMIT = 4;

MockEntity::MockEntity(dlb_adm_entity_id entityID)
    : ModelEntity(entityID)
{
    mNameLimit = MOCK_NAME_LIMIT;
}

bool MockEntity::AddLabel(const char *name, const char *language)
{
    return ModelEntity::AddLabel(name, language);
}

bool MockEntity::AddLabel(const std::string &name, const std::string &language)
{
    return ModelEntity::AddLabel(name, language);
}


class CoreModelTest : public testing::Test
{
protected:
    MockEntity *mMockEntity;

    virtual void SetUp()
    {
        mMockEntity = nullptr;
    }

    virtual void TearDown()
    {
        if (mMockEntity != nullptr)
        {
            delete mMockEntity;
            mMockEntity = nullptr;
        }
    }
};

static const char LANG_EN[]      = "en";
static const char THE_NAME[]     = "The Name";
static const char ANOTHER_NAME[] = "Another Name";
static const char LABEL_1[]      = "Label 1";
static const char LABEL_2[]      = "Label 2";
static const char LABEL_3[]      = "Label 3";
static const char LABEL_4[]      = "Label 4";
static const char LABEL_5[]      = "Label 5";

TEST_F(CoreModelTest, MockNaming)
{
    mMockEntity = new MockEntity(DLB_ADM_NULL_ENTITY_ID);
    EntityName entityName;

    EXPECT_FALSE(mMockEntity->HasName());
    EXPECT_FALSE(mMockEntity->GetName(entityName, 0));

    EXPECT_TRUE(mMockEntity->AddName(THE_NAME, LANG_EN));
    EXPECT_EQ(1u, mMockEntity->GetNameCount());
    EXPECT_TRUE(mMockEntity->HasName());
    EXPECT_TRUE(mMockEntity->GetName(entityName, 0));
    EXPECT_EQ(std::string(THE_NAME), entityName.GetName());
    EXPECT_TRUE(entityName.HasLanguage());
    EXPECT_EQ(std::string(LANG_EN), entityName.GetLanguage());

    EXPECT_FALSE(mMockEntity->AddName(ANOTHER_NAME, LANG_EN));
    EXPECT_EQ(1u, mMockEntity->GetNameCount());
}

TEST_F(CoreModelTest, MockLabeling)
{
    mMockEntity = new MockEntity(DLB_ADM_NULL_ENTITY_ID);

    EXPECT_TRUE(mMockEntity->AddLabel(LABEL_1, LANG_EN));
    EXPECT_FALSE(mMockEntity->AddName(THE_NAME, LANG_EN));  // Can't add name after label
    EXPECT_TRUE(mMockEntity->AddLabel(LABEL_2, LANG_EN));
    EXPECT_TRUE(mMockEntity->AddLabel(LABEL_3, LANG_EN));
    EXPECT_TRUE(mMockEntity->AddLabel(LABEL_4, LANG_EN));
    EXPECT_FALSE(mMockEntity->AddLabel(LABEL_5, LANG_EN));  // Too many
}

TEST_F(CoreModelTest, MockNamingAndLabeling)
{
    mMockEntity = new MockEntity(DLB_ADM_NULL_ENTITY_ID);

    EXPECT_TRUE(mMockEntity->AddName(THE_NAME, LANG_EN));
    EXPECT_TRUE(mMockEntity->AddLabel(LABEL_1, LANG_EN));
    EXPECT_TRUE(mMockEntity->AddLabel(LABEL_2, LANG_EN));
    EXPECT_TRUE(mMockEntity->AddLabel(LABEL_3, LANG_EN));
    EXPECT_FALSE(mMockEntity->AddLabel(LABEL_4, LANG_EN));  // Too many
}