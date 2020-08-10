def prepare_test(test_directory):
    import os
    from shutil import copyfile

    cs_xml = "/etc/columnstore/Columnstore.xml"
    if os.environ.get("COLUMNSTORE_XML_DIR") is not None:
        cs_xml=os.environ.get("COLUMNSTORE_XML_DIR")+"/Columnstore.xml"

    print("cs_xml file %s" %(cs_xml))

    if os.path.exists(cs_xml):
       copyfile(cs_xml,os.path.join(test_directory,'/Columnstore.xml'))
    else:
        raise Exception("Columnstore.xml configuration '%s' could not be copied to test directory" % (cs_xml,))
    
    return

def cleanup_test(test_directory):
    import os
    if os.path.exists(os.path.join(test_directory,'/Columnstore.xml')):
        os.remove(os.path.join(test_directory,'/Columnstore.xml'))
    return
