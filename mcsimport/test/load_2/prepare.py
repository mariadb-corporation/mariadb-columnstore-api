def prepare_test(test_directory):
    import os, csv
    rows = 50000000

    # generate input.csv
    with open(os.path.join(test_directory,'input.csv'), 'wb') as input:
        inWriter = csv.writer(input, delimiter=',')
        r = 0
        while r < rows:
            inWriter.writerow([r,rows-r,rows+r])
            r += 1
    
    # copy input.csv to expected.csv
    if os.name == 'nt': # on Windows we can't symlink input.csv to expected.csv so we have to generate it
        with open(os.path.join(test_directory,'expected.csv'), 'wb') as expected:
            r = 0
            while r < rows:
                expected.write('\n')
                r += 1
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
