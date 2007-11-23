-- Copyright (c) 1991-2002, The Numerical ALgorithms Group Ltd.
-- All rights reserved.
-- Copyright (C) 2007, Gabriel Dos Reis.
-- All rights reserved.
--
-- Redistribution and use in source and binary forms, with or without
-- modification, are permitted provided that the following conditions are
-- met:
--
--     - Redistributions of source code must retain the above copyright
--       notice, this list of conditions and the following disclaimer.
--
--     - Redistributions in binary form must reproduce the above copyright
--       notice, this list of conditions and the following disclaimer in
--       the documentation and/or other materials provided with the
--       distribution.
--
--     - Neither the name of The Numerical ALgorithms Group Ltd. nor the
--       names of its contributors may be used to endorse or promote products
--       derived from this software without specific prior written permission.
--
-- THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
-- IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
-- TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
-- PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
-- OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
-- EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
-- PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
-- PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
-- LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
-- NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
-- SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


import '"macros"
)package "BOOT"

d03edf() ==
  htInitPage('"D03EDF - Elliptic PDE, solution of finite difference equations by a multigrid technique ",nil)
  htMakePage '(
    (domainConditions 
       (isDomain EM $EmptyMode)
       (isDomain F  (Float)))
    (text . "\windowlink{Manual Page}{manpageXXd03edf} for this routine ")
    (text . "\newline ")
    (text . "\lispwindowlink{Browser operation page}{(|oPageFrom| '|d03edf| '|NagPartialDifferentialEquationsPackage|)} for this routine")
    (text . "\newline \horizontalline ")
    (text . "\newline ")
    (text . "D03EDF solves, by multigrid iteration, the seven point scheme ")
    (text . "\newline \htbitmap{d03edf} \newline which arises from the  ")
    (text . "discretization of an elliptic partial differential equation of  ")
    (text . "the form \center{\htbitmap{d03edf1}} and its boundary conditions")
    (text . ", defined on a rectangular region. This we can write in matrix ")
    (text . "form as \newline \center{{\it Au =f}}")
    (text . "\blankline")
    (text . "\newline ")
    (text . "Read the input file to see the example program. ")
    (text . "\newline ")
    (text . "\menuitemstyle{}\tab{2} ")
    (text . "\spadcommand{)read d03edf \bound{s0}} ")
    (text . "\blankline")
    (text . "\newline ")
    (text . "If you would like to enter a problem, ")
    (text . "how would you like to input the matrices? ")
    (radioButtons matrix
        ("" "  By entering individual entries" long)
        ("" "  By entering matrix names already defined on the command line" short)))
  htMakeDoneButton('"Continue", 'd03edfControl)
  htShowPage()

d03edfControl(htPage) ==
  type := htpButtonValue(htPage,'matrix)
  if (type = 'long) then 
    d03edfLong()
  else
    d03edfShort()

d03edfLong() ==
  htInitPage('"D03EDF - Elliptic PDE, solution of finite difference equations by a multigrid technique ",nil)
  htMakePage '(
    (domainConditions 
       (isDomain EM $EmptyMode)
       (isDomain F  (Float)))
    (text . "\blankline")
    (text . "\newline ")
    (text . "\menuitemstyle{}\tab{2} ")
    (text . "Number of interior grid points in the {\it x}-direction ")
    (text . "{\it ngx}: ")
    (text . "\newline\tab{2} ")
    (bcStrings (10 3 ngx PI))
    (text . "\blankline")
    (text . "\newline ")
    (text . "\menuitemstyle{}\tab{2} ")
    (text . "Number of interior grid points in the {\it y}-direction ")
    (text . "{\it ngy}: ")
    (text . "\newline\tab{2} ")
    (bcStrings (10 3 ngy PI))
    (text . "\blankline")
    (text . "\newline ")
    (text . "\menuitemstyle{}\tab{2} ")
    (text . "First dimension of A, {\it lda}: ")
    (text . "\newline\tab{2} ")
    (bcStrings (10 22 lda PI))
    (text . "\blankline")
    (text . "\newline ")
    (text . "\menuitemstyle{}\tab{2} ")
    (text . "Maximum permitted number of multigrid iterations, {\it maxit}: ")
    (text . "\newline\tab{2} ")
    (bcStrings (10 1 maxit PI))
    (text . "\blankline")
    (text . "\newline ")
    (text . "\menuitemstyle{}\tab{2} ")
    (text . "Tolerance required, {\it acc}: ")
    (text . "\newline\tab{2} ")
    (bcStrings (10 "1.0e-4" acc F))
    (text . "\blankline")
    (text . "\newline ")
    (text . "\menuitemstyle{}\tab{2} ")
    (text . "\newline Output of printed information for routine {\it iout}:")
    (radioButtons iout
        ("" " 0 - no output" zero)
        ("" " 1 - the solution \htbitmap{uij} {\it i} = 1,2,...,NGX; {\it j} = 1,2,...,NGY" one)
        ("" " 2 - residual 2-norm after each iteration " two)
        ("" " 3 - as for iout = 1 & iout = 2" three)
        ("" " 4 - as for iout = 3, plus the final residual" four)
        ("" " 5 - as for iout = 4, plus initial elements of A & RHS" five)
        ("" " 6 - as for iout = 5, plus Galerkin coarse grid approximations" six)
        ("" " 7 - as for iout = 6, plus the incomplete Crout decompositions" seven)
        ("" " 8 - as for iout = 7, plus the residual after each iteration" eight))
    (text . "\blankline")
    (text . "\newline ")
    (text . "\menuitemstyle{}\tab{2} ")
    (text . "\newline Ifail value:")
    (radioButtons ifail
        ("" " -1, Print error messages" ifail_minusOne)
        ("" "  1, Suppress error messages" ifail_one)))
  htMakeDoneButton('"Continue", 'd03edfSolve)
  htShowPage()


d03edfSolve htPage ==
  ngx :=
    $bcParseOnly => PARSE_-INTEGER htpLabelInputString(htPage, 'ngx)
    objValUnwrap htpLabelSpadValue(htPage, 'ngx)
  ngy :=
    $bcParseOnly => PARSE_-INTEGER htpLabelInputString(htPage, 'ngy)
    objValUnwrap htpLabelSpadValue(htPage, 'ngy)
  lda :=
    $bcParseOnly => PARSE_-INTEGER htpLabelInputString(htPage, 'lda)
    objValUnwrap htpLabelSpadValue(htPage, 'lda)
  maxit :=
    $bcParseOnly => PARSE_-INTEGER htpLabelInputString(htPage, 'maxit)
    objValUnwrap htpLabelSpadValue(htPage, 'maxit)
  acc := htpLabelInputString(htPage,'acc)
  control := htpButtonValue(htPage,'iout)
  iout :=
    control = 'zero => '0
    control = 'one => '1
    control = 'two => '2
    control = 'three => '3
    control = 'four => '4
    control = 'five => '5
    control = 'six => '6
    control = 'seven => '7
    '8
  error := htpButtonValue(htPage,'ifail)
  ifail :=
    error = 'ifail_one => '1
    '-1
  aList := 
    "append"/[fa(i) for i in 1..lda] where fa(i) ==
       labelList := 
         "append"/[fb(i,j) for j in 1..7] where fb(i,j) ==
            anam := INTERN STRCONC ('"a",STRINGIMAGE i,STRINGIMAGE j)
            [['bcStrings,[5, 0, anam, 'F]]]
       prefix := ('"\newline ")
       labelList := [['text,:prefix],:labelList]
  middle := ('"\blankline \menuitemstyle{} \tab{2} Enter the matrix ")
  middle := STRCONC(middle,'"{\it rhs(lda)}: \newline ")
  rList := 
    "append"/[fc(i) for i in 1..lda] where fc(i) ==
        rnam := INTERN STRCONC ('"r",STRINGIMAGE i)
        [['bcStrings,[6, "0.0", rnam, 'F]]]
  rList :=  [['text,:middle],:rList]
  mid:= ('"\blankline \menuitemstyle{} \tab{2} Enter the matrix ")
  mid := STRCONC(mid,'" {\it ub(ngx*ngy)}: \newline ")
  uList := 
    "append"/[fd(i) for i in 1..(ngx*ngy)] where fd(i) ==
        unam := INTERN STRCONC ('"u",STRINGIMAGE i)
        [['bcStrings,[6, 0, unam, 'F]]]
  uList :=  [['text,:mid],:uList]
  equationPart := [
     '(domainConditions 
        (isDomain EM $EmptyMode)
          (isDomain S (String))
            (isDomain F (Float))
              (isDomain I (Integer))),
                :aList,:rList,:uList]
  page := htInitPage('"D03EDF - Elliptic PDE, solution of finite difference equations by a multigrid technique ",nil)
  htSay '"\menuitemstyle{}\tab{2} "
  htSay '"Enter the matrix {\it a(lda,7)}: "
  htSay '"\newline "
  htMakePage equationPart
  htMakeDoneButton('"Continue",'d03edfLongGen)
  htpSetProperty(page,'ngx,ngx)
  htpSetProperty(page,'ngy,ngy)
  htpSetProperty(page,'lda,lda)
  htpSetProperty(page,'maxit,maxit)
  htpSetProperty(page,'acc,acc)
  htpSetProperty(page,'iout,iout)
  htpSetProperty(page,'ifail,ifail)
  htpSetProperty(page,'inputArea, htpInputAreaAlist htPage)
  htShowPage()

d03edfLongGen htPage ==
  ngx := htpProperty(htPage, 'ngx)
  ngy := htpProperty(htPage, 'ngy)
  lda := htpProperty(htPage, 'lda)
  maxit := htpProperty(htPage, 'maxit)
  acc := htpProperty(htPage, 'acc)
  iout := htpProperty(htPage, 'iout)
  ifail := htpProperty(htPage, 'ifail)
  alist := htpInputAreaAlist htPage
  y := alist
  for i in 1..(ngx*ngy) repeat
    utemp := STRCONC((first y).1," ")
    uList := [utemp,:uList]
    y := rest y
  ustring := bcwords2liststring uList
  for i in 1..lda repeat
    rtemp := STRCONC((first y).1," ")
    rList := [rtemp,:rList]
    y := rest y
  rstring := bcwords2liststring rList
  for i in 1..lda repeat
    for j in 1..7 repeat
      v := STRCONC((first y).1," ")
      rowList := [v,:rowList]
      y := rest y
    vList := [:vList,rowList]
    rowList := []
  vList := reverse vList
  astring := bcwords2liststring [bcwords2liststring x for x in vList]
  prefix := STRCONC("d03edf(", STRINGIMAGE ngx,", ",STRINGIMAGE ngy,", ")
  prefix := STRCONC(prefix,STRINGIMAGE lda,", ",STRINGIMAGE maxit,", ",acc)
  mid := STRCONC(", ",STRINGIMAGE iout,", ",astring,"::Matrix DoubleFloat,[")
  mid := STRCONC(mid,rstring,"],[",ustring,"],",STRINGIMAGE ifail,")")
  linkGen STRCONC(prefix,mid)
  
d03edfShort() ==
  htInitPage('"D03EDF - Elliptic PDE, solution of finite difference equations by a multigrid technique ",nil)
  htMakePage '(
    (domainConditions 
       (isDomain EM $EmptyMode)
       (isDomain F  (Float)))
    (text . "\blankline")
    (text . "\newline ")
    (text . "\menuitemstyle{}\tab{2} ")
    (text . "Number of interior grid points in the {\it x}-direction ")
    (text . "\htbitmap{nx}: ")
    (text . "\newline\tab{2} ")
    (bcStrings (10 0 ngx PI))
    (text . "\blankline")
    (text . "\newline ")
    (text . "\menuitemstyle{}\tab{2} ")
    (text . "Number of interior grid points in the {\it y}-direction ")
    (text . "\htbitmap{ny}: ")
    (text . "\newline\tab{2} ")
    (bcStrings (10 0 ngy PI))
    (text . "\blankline")
    (text . "\newline ")
    (text . "\menuitemstyle{}\tab{2} ")
    (text . "First dimension of A, {\it lda}: ")
    (text . "\newline\tab{2} ")
    (bcStrings (10 0 lda PI))
    (text . "\blankline")
    (text . "\newline ")
    (text . "\menuitemstyle{}\tab{2} ")
    (text . "Name of the array {\it a(lda,7)} defined on the command line: ")
    (text . "\newline\tab{2} ")
    (bcStrings (10 "a" a EM))
    (text . "\blankline")
    (text . "\newline ")
    (text . "\menuitemstyle{}\tab{2} ")
    (text . "Name of the array {\it rhs(lda)} defined on the command line: ")
    (text . "\newline\tab{2} ")
    (bcStrings (10 "rhs" rhs EM))
    (text . "\blankline")
    (text . "\newline ")
    (text . "\menuitemstyle{}\tab{2} ")
    (text . "Name of the array {\it ub(ngx*ngy)} defined on the command line:")
    (text . "\newline\tab{2} ")
    (bcStrings (10 "ub" ub EM))
    (text . "\blankline")
    (text . "\newline ")
    (text . "\menuitemstyle{}\tab{2} ")
    (text . "Maximum permitted number of multigrid iterations, {\it maxit}: ")
    (text . "\newline\tab{2} ")
    (bcStrings (10 0 maxit PI))
    (text . "\blankline")
    (text . "\newline ")
    (text . "\menuitemstyle{}\tab{2} ")
    (text . "Tolerance required, {\it acc}: ")
    (text . "\newline\tab{2} ")
    (bcStrings (10 "1.0e-4" acc F))
    (text . "\blankline")
    (text . "\newline ")
    (text . "\menuitemstyle{}\tab{2} ")
    (text . "\newline Output of printed information for routine {\it iout}:")
    (radioButtons iout
        ("" " 0 - no output" zero)
        ("" " 1 - the solution \htbitmap{uij} {\it i} = 1,2,...,NGX; {\it j} = 1,2,...,NGY" one)
        ("" " 2 - residual 2-norm after each iteration " two)
        ("" " 3 - as for iout = 1 & iout = 2" three)
        ("" " 4 - as for iout = 3, plus the final residual" four)
        ("" " 5 - as for iout = 4, plus initial elements of A & RHS" five)
        ("" " 6 - as for iout = 5, plus Galerkin coarse grid approximations" six)
        ("" " 7 - as for iout = 6, plus the incomplete Crout decompositions" seven)
        ("" " 8 - as for iout = 7, plus the residual after each iteration" eight))
    (text . "\blankline")
    (text . "\newline ")
    (text . "\menuitemstyle{}\tab{2} ")
    (text . "\newline Ifail value:")
    (radioButtons ifail
        ("" " -1, Print error messages" ifail_minusOne)
        ("" "  1, Suppress error messages" ifail_one)))
  htMakeDoneButton('"Continue", 'd03edfShortGen)
  htShowPage()


d03edfShortGen htPage ==
  ngx :=
    $bcParseOnly => PARSE_-INTEGER htpLabelInputString(htPage, 'ngx)
    objValUnwrap htpLabelSpadValue(htPage, 'ngx)
  ngy :=
    $bcParseOnly => PARSE_-INTEGER htpLabelInputString(htPage, 'ngy)
    objValUnwrap htpLabelSpadValue(htPage, 'ngy)
  lda :=
    $bcParseOnly => PARSE_-INTEGER htpLabelInputString(htPage, 'lda)
    objValUnwrap htpLabelSpadValue(htPage, 'lda)
  maxit :=
    $bcParseOnly => PARSE_-INTEGER htpLabelInputString(htPage, 'maxit)
    objValUnwrap htpLabelSpadValue(htPage, 'maxit)
  a := htpLabelInputString(htPage, 'a)
  rhs := htpLabelInputString(htPage, 'rhs)
  ub := htpLabelInputString(htPage, 'ub)
  acc := htpLabelInputString(htPage,'acc)
  control := htpButtonValue(htPage,'iout)
  iout :=
    control = 'zero => '0
    control = 'one => '1
    control = 'two => '2
    control = 'three => '3
    control = 'four => '4
    control = 'five => '5
    control = 'six => '6
    control = 'seven => '7
    '8
  error := htpButtonValue(htPage,'ifail)
  ifail :=
    error = 'ifail_one => '1
    '-1
  prefix := STRCONC("d03edf(", STRINGIMAGE ngx,", ",STRINGIMAGE ngy,", ")
  prefix := STRCONC(prefix,STRINGIMAGE lda,", ",STRINGIMAGE maxit,", ",acc)
  mid := STRCONC(", ",STRINGIMAGE iout,", ",a,", ")
  mid := STRCONC(mid,rhs,", ",ub,", ",STRINGIMAGE ifail,")")
  linkGen STRCONC(prefix,mid)



d03eef() ==
  htInitPage('"D03EEF - Discretize a 2nd order elliptic PDE on a rectangle",nil)
  htMakePage '(
    (domainConditions 
       (isDomain EM $EmptyMode)
       (isDomain F  (Float)))
    (text . "\windowlink{Manual Page}{manpageXXd03eef} for this routine ")
    (text . "\newline ")
    (text . "\lispwindowlink{Browser operation page}{(|oPageFrom| '|d03eef| '|NagPartialDifferentialEquationsPackage|)} for this routine")
    (text . "\newline \horizontalline ")
    (text . "\newline ")
    (text . "D03EEF discretizes a second order linear elliptic partial ")
    (text . "differential equation of the form \center{\htbitmap{d03eef}} ")
    (text . "on a rectangular region \newline \tab{2} ")
    (text  . "{\it x}a \htbitmap{less=} {\it x}  \htbitmap{less=} {\it x}b ")
    (text . "\newline \tab{2} {\it y}a \htbitmap{less=} {\it y} ")
    (text . "\htbitmap{less=} {\it y}b \newline subject to the boundary ")
    (text . "conditions of the form \newline \htbitmap{d03eef1} \newline ")
    (text . "where {\it \delta U/ \delta n} denotes the outward pointing ")
    (text . "normal derivative on the boundary. The equation is said to be ")
    (text . "elliptic if \center{\htbitmap{d03eef2}} \newline for all points ")
    (text . "in the rectangular region. The seven-diagonal linear equations ")
    (text . "produced are in a form suitable for passing directly to the ")
    (text . "multigrid routine D03EDF. \blankline ")
    (text . "The equation is discretized on a rectangular grid, with ")
    (text . "\htbitmap{nx} grid points in the {\it x}-direction and ")
    (text . "\htbitmap{ny} grid points in the {\it y}-direction. "))
  htMakeDoneButton('"Continue", 'd03eefInput)
  htShowPage()

d03eefInput() ==
  htInitPage('"D03EEF - Discretize a 2nd order elliptic PDE on a rectangle",nil)
  htMakePage '(
    (domainConditions 
       (isDomain EM $EmptyMode)
       (isDomain F  (Float)))
    (text . "\newline ")
    (text . "\menuitemstyle{}\tab{2} ")
    (text . "Enter the value {\it x}a, {\it xmin}: ")
    (text . "\newline\tab{2} ")
    (bcStrings (10 "0.0" xmin F))
    (text . "\blankline")
    (text . "\newline ")
    (text . "\menuitemstyle{}\tab{2} ")
    (text . "Enter the value {\it x}b, {\it xmax}: ")
    (text . "\newline\tab{2} ")
    (bcStrings (10 "1.0" xmax F))
    (text . "\blankline")
    (text . "\newline ")
    (text . "\menuitemstyle{}\tab{2} ")
    (text . "Enter the value {\it y}a, {\it ymin}: ")
    (text . "\newline\tab{2} ")
    (bcStrings (10 "0.0" ymin F))
    (text . "\blankline")
    (text . "\newline ")
    (text . "\menuitemstyle{}\tab{2} ")
    (text . "Enter the value {\it y}b, {\it ymax}: ")
    (text . "\newline\tab{2} ")
    (bcStrings (10 "1.0" ymax F))
    (text . "\blankline")
    (text . "\newline ")
    (text . "\menuitemstyle{}\tab{2} ")
    (text . "Number of interior grid points in the {\it x}-direction ")
    (text . "{\it ngx}: ")
    (text . "\newline\tab{2} ")
    (bcStrings (10 9 ngx PI))
    (text . "\blankline")
    (text . "\newline ")
    (text . "\menuitemstyle{}\tab{2} ")
    (text . "Number of interior grid points in the {\it y}-direction ")
    (text . "{\it ngy}: ")
    (text . "\newline\tab{2} ")
    (bcStrings (10 9 ngy PI))
    (text . "\blankline")
    (text . "\newline ")
    (text . "\menuitemstyle{}\tab{2} ")
    (text . "First dimension of A, {\it lda}: ")
    (text . "\newline\tab{2} ")
    (bcStrings (10 133 lda PI))
    (text . "\blankline")
    (text . "\newline ")
    (text . "\menuitemstyle{}\tab{2} ")
    (text . "\newline Which {\it scheme} would you like to use: ")
    (radioButtons scheme
        (" C" "  central differences" cent)
        (" U" "  upwind differences" up))
    (text . "\blankline")
    (text . "\newline ")
    (text . "\menuitemstyle{}\tab{2} ")
    (text . "\newline Ifail value:")
    (radioButtons ifail
        ("" " -1, Print error messages" minusOne)
        ("" "  1, Suppress error messages" one)))
  htMakeDoneButton('"Continue", 'd03eefSolve)
  htShowPage()



d03eefSolve htPage ==
  xmin := htpLabelInputString(htPage,'xmin)
  xmax := htpLabelInputString(htPage,'xmax)
  ymin := htpLabelInputString(htPage,'ymin)
  ymax := htpLabelInputString(htPage,'ymax)
  ngx :=
    $bcParseOnly => PARSE_-INTEGER htpLabelInputString(htPage, 'ngx)
    objValUnwrap htpLabelSpadValue(htPage, 'ngx)
  ngy :=
    $bcParseOnly => PARSE_-INTEGER htpLabelInputString(htPage, 'ngy)
    objValUnwrap htpLabelSpadValue(htPage, 'ngy)
  lda :=
    $bcParseOnly => PARSE_-INTEGER htpLabelInputString(htPage, 'lda)
    objValUnwrap htpLabelSpadValue(htPage, 'lda)
  diff := htpButtonValue(htPage,'scheme)
  scheme :=
    diff = 'cent => '"C"
    '"U"
  error := htpButtonValue(htPage,'ifail)
  ifail :=
    error = 'one => '1
    '-1
  d03eefDefaultSolve(htPage,xmin,xmax,ymin,ymax,ngx,ngy,lda,scheme,ifail)

d03eefDefaultSolve(htPage,xmin,xmax,ymin,ymax,ngx,ngy,lda,scheme,ifail) ==
  page := htInitPage('"D03EEF - Discretize a 2nd order elliptic PDE on a rectangle",nil)
  htMakePage '(
    (domainConditions 
       (isDomain EM $EmptyMode)
       (isDomain F  (Float)))
    (text . "Please enter the values of \alpha to \psi to construct PDEF.")
    (text . "\newline ")
    (text . "\menuitemstyle{}\tab{2} ")
    (text . "\alpha (x,y): \tab{10} ")
    (bcStrings (46 1 alpha F))
    (text . "\blankline ")
    (text . "\newline ")
    (text . "\menuitemstyle{}\tab{2} ")
    (text . "\beta (x,y): \tab{10} ")
    (bcStrings (46 0 beta F))
    (text . "\blankline ")
    (text . "\newline ")
    (text . "\menuitemstyle{}\tab{2} ")
    (text . "\gamma (x,y): \tab{10} ")
    (bcStrings (46 1 gamma F))
    (text . "\blankline ")
    (text . "\newline ")
    (text . "\menuitemstyle{}\tab{2} ")
    (text . "\delta (x,y): \tab{10} ")
    (bcStrings (46 50 delta F))
    (text . "\blankline")
    (text . "\newline ")
    (text . "\menuitemstyle{}\tab{2} ")
    (text . "\epsilon (x,y): \tab{10} ")
    (bcStrings (46 50 eps F))
    (text . "\blankline ")
    (text . "\newline ")
    (text . "\menuitemstyle{}\tab{2} ")
    (text . "\phi (x,y): \tab{10} ")
    (bcStrings (46 0 phi F))
    (text . "\blankline ")
    (text . "\newline ")
    (text . "\menuitemstyle{}\tab{2} ")
    (text . "\psi (x,y): \tab{10} ")    
    (bcStrings (55 "-2*sin(X)*sin(Y) + 50*cos(X)*sin(Y) +50*sin(X)*cos(Y)" psi EM))
    (text . "\blankline ")
    (text . "Please enter the boundary conditions a(x,y), b(x,y), and c(x,y) ")
    (text . "for the top, bottom, left and right hand sides, to construct ")
    (text . "BNDY. \blankline")
    (text . "\newline ")
    (text . "\menuitemstyle{}\tab{2} ")
    (text . "Bottom boundary conditions: ")
    (text . "\newline a(x,y): \tab{10} ")
    (bcStrings (46 0 a11 F))
    (text . "\newline b(x,y): \tab{10} ")
    (bcStrings (46 1 a12 F))
    (text . "\newline c(x,y): \tab{10} ")
    (bcStrings (46 "-sin(X)" a13 EM))
    (text . "\blankline ")
    (text . "\menuitemstyle{}\tab{2} ")
    (text . "Right boundary conditions: ")
    (text . "\newline a(x,y): \tab{10} ")
    (bcStrings (46 1 a21 F))
    (text . "\newline b(x,y): \tab{10} ")
    (bcStrings (46 0 a22 F))
    (text . "\newline c(x,y): \tab{10} ")
    (bcStrings (46 "sin(X)*sin(Y)" a23 EM))
    (text . "\blankline ")
    (text . "\menuitemstyle{}\tab{2} ")
    (text . "Top boundary conditions: ")
    (text . "\newline a(x,y): \tab{10} ")
    (bcStrings (46 1 a31 F))
    (text . "\newline b(x,y): \tab{10} ")
    (bcStrings (46 0 a32 F))
    (text . "\newline c(x,y): \tab{10} ")
    (bcStrings (46 "sin(X)*sin(Y)" a33 EM))
    (text . "\blankline ")
    (text . "\menuitemstyle{}\tab{2} ")
    (text . "Left boundary conditions: ")
    (text . "\newline a(x,y): \tab{10} ")
    (bcStrings (46 0 a41 F))
    (text . "\newline b(x,y): \tab{10} ")
    (bcStrings (46 1 a42 F))
    (text . "\newline c(x,y): \tab{10} ")
    (bcStrings (46 "-sin(Y)" a43 EM)))
  htMakeDoneButton('"Continue",'d03eefGen)
  htpSetProperty(page,'xmin,xmin)
  htpSetProperty(page,'xmax,xmax)
  htpSetProperty(page,'ymin,ymin)
  htpSetProperty(page,'ymax,ymax)
  htpSetProperty(page,'ngx,ngx)
  htpSetProperty(page,'ngy,ngy)
  htpSetProperty(page,'lda,lda)
  htpSetProperty(page,'scheme,scheme)
  htpSetProperty(page,'ifail,ifail)
  htpSetProperty(page,'inputArea, htpInputAreaAlist htPage)
  htShowPage()

d03eefGen htPage ==
  xmin := htpProperty(htPage, 'xmin)
  xmax := htpProperty(htPage, 'xmax)
  ymin := htpProperty(htPage, 'ymin)
  ymax := htpProperty(htPage, 'ymax)
  ngx := htpProperty(htPage, 'ngx)
  ngy := htpProperty(htPage, 'ngy)
  lda := htpProperty(htPage, 'lda)
  scheme := htpProperty(htPage, 'scheme)
  ifail := htpProperty(htPage, 'ifail)
  alist := htpInputAreaAlist htPage
  y := alist
  for i in 1..4 repeat
    for j in 1..3 repeat
      v := STRCONC((first y).1," ")
      rowList := [v,:rowList]
      y := rest y
    vList := [:vList,rowList]
    rowList := []
  vList := reverse vList
  astring := bcwords2liststring [bcwords2liststring x for x in vList]
  for i in 1..7 repeat
    utemp := STRCONC((first y).1," ")
    uList := [utemp,:uList]
    y := rest y
  ustring := bcwords2liststring uList
  prefix := STRCONC("d03eef(",xmin,", ",xmax,", ",ymin,", ",ymax,", ")
  prefix := STRCONC(prefix,STRINGIMAGE ngx,", ",STRINGIMAGE ngy,", ")
  prefix := STRCONC(prefix,STRINGIMAGE lda,",_"",scheme,"_", ")
  prefix := STRCONC(prefix,STRINGIMAGE ifail,", (",ustring)
  prefix := STRCONC(prefix,"::Vector Expression Float)::ASP73('PDEF),(")
  prefix := STRCONC(prefix,astring,"::Matrix Expression Float)::ASP74('BNDY))")
  linkGen prefix
  
d03faf() ==
  htInitPage('"D03FAF - Elliptic PDE, Helmholtz equation, 3-D Cartesian co-ordinates",nil)
  htMakePage '(
    (domainConditions 
       (isDomain EM $EmptyMode)
       (isDomain F  (Float)))
    (text . "\windowlink{Manual Page}{manpageXXd03faf} for this routine ")
    (text . "\newline ")
    (text . "\lispwindowlink{Browser operation page}{(|oPageFrom| '|d03faf| '|NagPartialDifferentialEquationsPackage|)} for this routine")
    (text . "\newline \horizontalline ")
    (text . "\newline ")
    (text . "D03FAF solves the three-dimensional Helmholtz equation ")
    (text . "in cartesian co-ordinates: \center{\htbitmap{d03faf}} \newline ")
    (text . "This subroutine forms the system of linear equations resulting ")
    (text . "fom the standard seven-point finite difference equations, ")
    (text . "and then solves the system using a method based on the fast ")
    (text . "Fourier transform (FFT) described by Swartztrauber. ")
    (text . "\blankline")
    (text . "\newline ")
    (text . "Read the input file to see the example program. ")
    (text . "\newline ")
    (text . "\menuitemstyle{}\tab{2} ")
    (text . "\spadcommand{)read d03faf \bound{s0}} "))
  htShowPage()
