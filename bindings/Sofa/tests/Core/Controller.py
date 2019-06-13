# coding: utf8

import unittest
import gc
import Sofa

class MyController(Sofa.Controller):
        """This is my custom controller
           when init is called from Sofa this should call the python init function
        """        

        onAnimateEndEvent = "check that handleEvent falls back on onEvent even \
                             when a non-callable holds the name of an event"
        def __init__(self, *args, **kwargs):
            ## These are needed (and the normal way to override from a python class)
            Sofa.Controller.__init__(self, *args, **kwargs)
            print(" Python::__init__::"+str(self.name))
            self.inited = 0
            self.iterations = 0

        def __del__(self):
                print(" Python::__del__")
        
        def init(self):
                print(" Python::init() at "+str(self))
                self.inited += 1

        def onEvent(self, event):
                print(" Handling event " + str(event))

        def onAnimateBeginEvent(self, other):
                print(" Python::onAnimationBeginEvent() ("+str(other) + ")")
                self.iterations+=1


class Test(unittest.TestCase):
         def test_constructor(self):
                 c = Sofa.Controller()
                 c.init()
                 c.reinit()

         def test_constructorOverriden(self):
                 root = Sofa.Node("rootNode")
                 root.addObject(MyController(name="controller"))
                 root.controller.init()
                 root.controller.reinit()

         def test_constructorOverriddenWithArgs(self):
             root = Sofa.Node("rootNode")
             root.addObject(MyController("controller", "pval1", "pval2", "pval3"))

         def test_constructorOverriddenWithKWArgs(self):
             root = Sofa.Node("rootNode")
             root.addObject(MyController("controller", kval1="value1", kval2="value2", kval3="value3"))

         def test_methodOverriding(self):
                 """Test that a custom controller 'MyController' correctly adds attributes when overridden.
                 dynamically in its init and reinit function. And that after the
                 call the attributes are still available.
                 """
                 c = MyController(name="controller")

                 self.assertTrue( hasattr(c, "inited") )
                 c.init()

                 self.assertEqual( c.inited, 1 ) 
                 return c

         def test_events(self):
            """Test the event system."""
            node = Sofa.Node("root")
            node.addObject("DefaultAnimationLoop", name="loop")
            controller = node.addObject( MyController() )


            self.assertTrue( hasattr(controller, "iterations") )

            Sofa.Simulation.init(node)
            for i in range(10):
                Sofa.Simulation.animate(node, 0.01)

            self.assertTrue( hasattr(controller, "iterations") )
            self.assertEqual( controller.iterations, 10 )

         def test_bindingPersistance(self):
                 """Test that custom controllers correctly preserves attributes.
                    This can happens if the binding is not properly done and is loosing
                    the python part when go to/from sofa without having the sofa side
                    holding a reference to the python object.
                 """
                 node = Sofa.Node("root")
                 node.addObject( MyController(name="controller") )
                 node.init()

                 ## At this step we can validate that the python side is ok.
                 self.assertTrue( hasattr(node.controller, "inited") )
                 self.assertEqual( node.controller.inited, 1 )

                 gc.collect()

def getTestsName():
    suite = unittest.TestLoader().loadTestsFromTestCase(Test)
    return [ test.id().split(".")[2] for test in suite]

def runTests():
        import sys
        suite = None
        if( len(sys.argv) == 1 ):
            suite = unittest.TestLoader().loadTestsFromTestCase(Test)
        else:
            suite = unittest.TestSuite()
            suite.addTest(Test(sys.argv[1]))
        return unittest.TextTestRunner(verbosity=1).run(suite).wasSuccessful()

def createScene(rootNode):
        runTests()

