def prepare_test(test_directory):
    from base64 import b64encode
    import os, csv
    rows = 1000000

    # generate input.csv
    with open(os.path.join(test_directory,'input.csv'), 'wb') as input:
        inWriter = csv.writer(input, delimiter=',')
        r = 0
        while r < rows:
            s = b64encode(str(r))
            if len(s) > 8:
                s = s[0:7]
            inWriter.writerow([r,rows-r,rows+r,str(r),s])
            r += 1
    
    # copy input.csv to expected.csv
    if os.name == 'nt': # on Windows we can't symlink input.csv to expected.csv so we have to copy it
        from shutil import copyfile
        copyfile(os.path.join(test_directory,'input.csv'),os.path.join(test_directory,'expected.csv'))
    else:
        os.symlink(os.path.join(test_directory,'input.csv'),os.path.join(test_directory,'expected.csv'))
    
    return

def cleanup_test(test_directory):
    import os
    if os.path.exists(os.path.join(test_directory,'expected.csv')):
        if os.name == 'nt':
            os.remove(os.path.join(test_directory,'expected.csv'))
        else:
            os.unlink(os.path.join(test_directory,'expected.csv'))
    if os.path.exists(os.path.join(test_directory,'input.csv')):
        os.remove(os.path.join(test_directory,'input.csv'))
    return
