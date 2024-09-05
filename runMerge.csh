#!/bin/tcsh

#root -l -q -b 'mergeTrees.C("tree_pi0pippimeta__B4_M7_M17_030406.root", "pi0pippimeta__B4_M7_M17_Tree", "tree_pi0pippimeta__B4_M17_030406.root", "pi0pippimeta__B4_M17_Tree")'

#root -l -q -b 'testTreeGroup.cc("tree_pi0pippimeta__B4_M7_M17_030406.root", "pi0pippimeta__B4_M7_M17_Tree", "tree_pi0pippimeta__B4_M17_030406.root", "pi0pippimeta__B4_M17_Tree")'

#root -l -q -b 'testTreeGroup.cc("testing/firstStep_tree.root", "pi0pippimeta__B4_M17_Tree", "tree_pi0pippimeta__B4_030406.root", "pi0pippimeta__B4_Tree")'

#root -l -q -b 'testTreeGroup.cc("testing/firstStep_tree.root", "pi0pippimeta__B4_M17_Tree", "tree_pi0pippimeta__B4_M17_030406.root", "pi0pippimeta__B4_M17_Tree")'

# root -l -q -b 'testTreeGroup.cc("testing/oneBeam.root", "pi0pippimeta__B4_M17_Tree", "tree_pi0pippimeta__B4_030406.root", "pi0pippimeta__B4_Tree")'

# mv matchedTree.root constrainedMatched.root

# root -l -q -b 'testTreeGroup.cc("constrainedMatched.root", "pi0pippimeta__B4_Tree","testing/oneBeam.root", "pi0pippimeta__B4_M17_Tree")'

# mv matchedTree.root unconstrainedMatched.root

#root -l -q -b 'testTreeGroup.cc("constrainedMatched.root", "pi0pippimeta__B4_Tree", "testing/firstStep_tree.root", "pi0pippimeta__B4_M17_Tree")'

#root -l -q -b 'testTreeGroup.cc("tree_pi0pippimeta__B4_M17_030406.root", "pi0pippimeta__B4_M17_Tree", "tree_pi0pippimeta__B4_030406.root", "pi0pippimeta__B4_Tree")'

#(root -l -q -b 'testTreeGroup.cc("tree_pi0pippimeta__B4_M7_M17_030406.root", "pi0pippimeta__B4_M7_M17_Tree", "tree_pi0pippimeta__B4_M17_030406.root", "pi0pippimeta__B4_M17_Tree")' > tmp.txt ) >tmp_err.txt

#(root -l -q -b 'testTreeGroup.cc("testing/firstStep_tree.root", "pi0pippimeta__B4_M17_Tree", "tree_pi0pippimeta__B4_030406.root", "pi0pippimeta__B4_Tree")' > tmp.txt ) >tmp_err.txt

# (root -l -q -b 'testTreeGroup.cc("testing/oneBeam.root", "pi0pippimeta__B4_M17_Tree", "tree_pi0pippimeta__B4_030406.root", "pi0pippimeta__B4_Tree")' > tmp.txt ) >tmp_err.txt
# mv matchedTree.root constrainedMatched.root
# (root -l -q -b 'testTreeGroup.cc("constrainedMatched.root", "pi0pippimeta__B4_Tree", "testing/oneBeam.root", "pi0pippimeta__B4_M17_Tree" )' > tmp2.txt ) >tmp2_err.txt
# mv matchedTree.root unconstrainedMatched.root



#(root -l -q -b 'testTreeGroup.cc("tree_pi0pippimeta__B4_030406.root", "pi0pippimeta__B4_Tree", "testing/oneBeam.root", "pi0pippimeta__B4_M17_Tree" )' > tmp.txt ) >tmp_err.txt
(root -l -q -b 'testTreeGroup.cc("tree_pi0pippimeta__B4_030406.root", "pi0pippimeta__B4_Tree", "testing/oneBeam_weight.root", "pi0pippimeta__B4_M17_Tree" )' > tmp.txt ) >tmp_err.txt
mv matchedTree.root constrainedMatched.root
#(root -l -q -b 'testTreeGroup.cc("testing/oneBeam.root", "pi0pippimeta__B4_M17_Tree", "constrainedMatched.root", "pi0pippimeta__B4_Tree"  )' > tmp2.txt ) >tmp2_err.txt
(root -l -q -b 'testTreeGroup.cc("testing/oneBeam_weight.root", "pi0pippimeta__B4_M17_Tree", "constrainedMatched.root", "pi0pippimeta__B4_Tree"  )' > tmp2.txt ) >tmp2_err.txt
mv matchedTree.root unconstrainedMatched.root
