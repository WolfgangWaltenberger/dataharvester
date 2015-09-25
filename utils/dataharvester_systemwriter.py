#!/usr/bin/python

"""
.. module:: dataharvester_systemwriter.py
   :synopsis: a script that writes out some info about the machine 
              into a dataharvester file.
              
.. moduleauthor:: Wolfgang Waltenberger <wolfgang.waltenberger@gmail.com>

"""
    
import dataharvester

def write ( filename, tuplename, what=dataharvester.SystemWriter.Everything ):
    w=dataharvester.SystemWriter( filename )
    w.save ( what, tuplename )
    dataharvester.Writer_close()

if __name__ == '__main__':
    import argparse, types
    argparser = argparse.ArgumentParser(description=
            'copy a root file, turning trees into hierarchic trees')
    argparser.add_argument ( '-f', '--file', nargs='?', help='the output file', type=types.StringType, default='stdout' )
    argparser.add_argument ( '-t', '--tuple', nargs='?', help='the tuple name', type=types.StringType, default='System' )
    argparser.add_argument( '-V','--version', action='version', version=dataharvester.version() )
    args=argparser.parse_args()

    write ( args.file, args.tuple )

