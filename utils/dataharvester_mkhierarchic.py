#!/usr/bin/python

"""
.. module:: dataharvester_mkhierarchic.py
   :synopsis: a script that turns a flat ttree into a hierarchic one.
              currently very hardcoded for the cms/pmssm13 scan,
              but can easliy be generalized when needed.
              
.. moduleauthor:: Wolfgang Waltenberger <wolfgang.waltenberger@gmail.com>

"""

import ROOT
import sys
import dataharvester

def _copy ( r, w, clone ):
    """ clone tree from read file to write file """
    tree=r.Get(clone)
    ctree=tree.CloneTree()
    w.cd()
    ctree.Write()

def copy_flat ( inf, outf, mode="recreate", verbose=False, removes=[] ):
    """ copies all trees from inf to outf """
    r=ROOT.TFile(inf)
    w=ROOT.TFile(outf,mode )
    ## first off, we copy all trees.
    keys=r.GetListOfKeys()
    for i in range(keys.GetEntries() ):
        name=keys.At(i).GetName()
        if name in removes:
            if verbose: print "wont copy flat:",name
            continue
        if verbose: print "copy flat:",name
        _copy ( r, w, name )
    w.Close()

def convert ( inf="in.root", outf="out.root",
        flatree="flatelements7", hiertree="elements7",
        indicators=["pointnr"], scalars=["pointnr"],
        vectors=["constraint","mass","compressed","nmothers","xsec_pb",
                 "lb1","lb2","mothers", "nallmothers" ],
        mode="update", verbose=False ):
    """ convert from flat to hierarchic """
    if verbose:
        print "create %s from %s" % ( hiertree, flatree )
    r=ROOT.TFile(inf)
    w=ROOT.TFile(outf, mode )
    ttree=r.Get( flatree )
    if ttree==None:
        print "[dataharvester_mkhierarchic] error tree",flatree,"not found."
        sys.exit(0)
    ttree.GetEntry(0)
    containers={}
    for arname in vectors:
        leaf=ttree.GetLeaf(arname)
        if leaf==None:
            print "[dataharvester_mkhierarchic] error leaf",arname,"not found."
            sys.exit(0)
        tpe=ttree.GetLeaf(arname).GetTypeName()
        if tpe=="Char_t": tpe="string"
        containers[arname]=ROOT.vector(tpe)()
    struct="struct MyStruct{ "
    for name in scalars:
        leaf=ttree.GetLeaf(name)
        if leaf==None:
            print "[dataharvester_mkhierarchic] error leaf",name,"not found."
            sys.exit(0)
        tpe=ttree.GetLeaf(name).GetTypeName() 
        struct+=tpe+" "+name+"; "
    struct+="}; s=MyStruct();"
    ROOT.gROOT.ProcessLine ( struct )
    from ROOT import s
    rtree = ROOT.TTree( hiertree, hiertree )
    letters = { "Int_t": "I", "Float_t": "F" }
    for name in scalars:
        tpe=ttree.GetLeaf(name).GetTypeName() 
        if not tpe in letters:
            print "Error. Dont yet know how to handle",tpe
            sys.exit()
        tpeLetter=letters[tpe]
        rtree.Branch ( name, ROOT.AddressOf(s,name),name+"/"+tpeLetter )
    for arname in vectors:
        rtree.Branch ( arname, containers[arname] )
    currentindicator=0
    lastindicators={}
    for ii in indicators:
        lastindicators[ii]=int ( ttree.GetLeaf(ii).GetValue() )
    ne=ttree.GetEntries()
    for ti in range(ne):
        ttree.GetEntry(ti)
        tmpindicators={}
        for ii in indicators:
            tmpindicators[ii]=int ( ttree.GetLeaf(ii).GetValue() )
        if lastindicators!=tmpindicators or ti==ne-1:
            rtree.Fill()
            for (key,cont) in containers.items():
                cont.clear()
            lastindicators=tmpindicators
        for name in scalars:
            if ttree.GetLeaf(name)!=None:
                cmd="s.%s=%s; " % ( name, ttree.GetLeaf(name).GetValue() )
                ROOT.gROOT.ProcessLine ( cmd )
        for var in vectors:
            tpe=ttree.GetLeaf(var).GetTypeName()
            if tpe=="Char_t": tpe="string"
            if ttree.GetLeaf(var)==None:
                print "var not found",var
            value=ttree.GetLeaf(var).GetValue()
            if tpe=="Int_t": value=int(value)
            if tpe=="Float_t": value=float(value)
            if tpe=="string": value=str( ttree.GetLeaf(var).GetValueString() )
            containers[var].push_back ( value )
    w.cd()
    rtree.Write()
    w.Close()

if __name__ == '__main__':
    import argparse, types
    argparser = argparse.ArgumentParser(description=
            'copy a root file, turning trees into hierarchic trees')
    argparser.add_argument ( '-i', '--input',
        help='input file', type=types.StringType, default="compressed.root" )
    argparser.add_argument ( '-o', '--output',
        help='output file', type=types.StringType, default="hierarchic.root" )
    argparser.add_argument ( '-s', '--scalars',
        help='scalar variables, comma separated list', type=types.StringType, default="pointnr" )
    argparser.add_argument ( '-I', '--indicators',
        help='indiactor variables, comma separated list. If empty, indicators:=scalars', type=types.StringType, default="" )
    argparser.add_argument ( '-vec', '--vectors',
        help='vector variables, comma separated list', type=types.StringType, 
        default="constraint,mass,compressed,nmothers,xsec_pb,lb1,lb2,mothers" )

    argparser.add_argument ( '-r', '--remove', help='remove flat trees that have been hierarchized', action='store_true' )
    argparser.add_argument ( '-V','--version', action='version', 
                             version=dataharvester.version() )
    argparser.add_argument ( '-v', '--verbose', help='be verbose',
        action='store_true' )
    args=argparser.parse_args()

    fromtree="flatelements"
    totree="elements"

    removes=[]
    if args.remove:
        removes.append ( fromtree )
    copy_flat ( args.input, args.output, "recreate", args.verbose, removes )

    indicators=args.indicators
    if indicators=="":
        indicators=args.scalars
    ## now for making hierarchic things
    ## FIXME next step is to get the types from ROOT.
    # scalars = [ "pointnr", "sqrts" ] ## these guys are one per row = scalars
    scalars=args.scalars.replace(" ","").replace("	","").split(",")
    indicators=indicators.replace(" ","").replace("	","").split(",")

    #vectors = [ "label", "mass", "compressed", "nmothers",
    #            "xsec_pb", "lb1", "lb2", "mothers" ] ## these guys are the vectors
    vectors=args.vectors.replace(" ","").replace("^I","").split(",")

    ## turn (flat) flatelements into (hierarchic) elements,
    ## every unique pair of pointnr and sqrts is a separate row.
    ## scalars variables are scalars, vectors are vectors
    convert ( args.input, args.output, fromtree, totree, indicators, scalars, 
              vectors, "update", args.verbose )
