target_sources(dlb_adm
    PRIVATE
        AttributeDescriptor.cpp
        AttributeDescriptor.h
        AttributeInitializers.h
        AttributeValue.cpp
        AttributeValue.h
        dlb_adm_xml_container.cpp
        dlb_adm_xml_container.h
        EntityContainer.cpp
        EntityContainer.h
        EntityDB.cpp
        EntityDB.h
        EntityDescriptor.cpp
        EntityDescriptor.h
        EntityInitializers.h
        EntityRecord.cpp
        EntityRecord.h
        EntityRelationship.h
        RelationshipContainer.h
        RelationshipDB.cpp
        RelationshipDB.h
        RelationshipDescriptor.cpp
        RelationshipDescriptor.h
        RelationshipInitializers.h
        RelationshipRecord.cpp
        RelationshipRecord.h
        TableIndex.h
        XMLBuffer.cpp
        XMLBuffer.h
        XMLContainer.cpp
        XMLContainer.h
        XMLContainerFlattener.cpp
        XMLContainerFlattener.h
        XMLContainerComplementaryFlattener.cpp
        XMLContainerComplementaryFlattener.h
        XMLReader.cpp
        XMLReader.h
        XMLReaderStack.cpp
        XMLReaderStack.h
        XMLWriter.cpp
        XMLWriter.h
)

target_include_directories(dlb_adm
    PRIVATE
        .
)
