Snit's Not Incr Tcl README.txt
-----------------------------------------------------------------

Snit is pure-Tcl object and megawidget framework.  See snit.html
for full details.

Snit is part of "tcllib", the standard Tcl Library.

Snit lives in "tcllib" now, but it is available separately at
http://www.wjduquette.com/snit.  If you have any questions, bug
reports, suggestions, or comments, feel free to contact me, Will
Duquette, at will@wjduquette.com; or, join the Snit mailing list (see
http://www.wjduquette.com/snit for details).

Changes in V0.93
--------------------------------------------------------------------

* Enhancement: Added the snit::typemethod and snit::method commands; 
  these allow typemethods and methods to be defined (and redefined)
  after the class already exists.  See the Snit man page for 
  details.

* Documentation fixes: a number of minor corrections were made to the
  Snit man page and FAQ.  Thanks to everyone who pointed them out,
  especially David S. Cargo.

* Bug fix: when using %AUTO% to create object names, the counter 
  will wrap around to 0 after it reaches (2^32 - 1), to prevent 
  integer overflow errors. (Credit Marty Backe)

* Bug fix: in a normal Tcl proc, the command

    variable ::my::namespace::var

  makes variable "::my::namespace::var" available to the proc under the 
  local name "var".  Snit redefines the "variable" command for use in
  instance methods, and had lost this behavior.  (Credit Jeff
  Hobbs)

* Bug fix: in some cases, the "info vars" instance method didn't
  include the "options" instance variable in its output.

* Fixed bug: in some cases the type command was created even if there 
  was an error defining the type.  The type command is now cleaned 
  up in these cases.  (Credit Andy Goth)


Changes in V0.92
--------------------------------------------------------------------

* Bug fix: In type methods, constructors, and methods, the "errorCode"
  of a thrown error was not propagated properly; no matter what it was
  set to, it always emerged as "NONE".

Changes in V0.91
--------------------------------------------------------------------

* Bug fix: On a system with both 0.9 and 0.81 installed, 
  "package require snit 0.9" would get snit 0.81.  Here's why: to me
  it was clear enough that 0.9 is later than 0.81, but to Tcl the 
  minor version number 9 is less than minor version number 81.
  From now on, all pre-1.0 Snit version numbers will have two
  digits.

* Bug fix: If a method or typemethod had an argument list which was
  broken onto multiple lines, the type definition would fail. It now
  works as expected.

* Added the "expose" statement; this allows you to expose an entire
  component as part of your type's public interface.  See the man page
  and the Snit FAQ list for more information.

* The "info" type and instance methods now take "string match"
  patterns as appropriate.

Changes in V0.9
--------------------------------------------------------------------

For specific changes, please see the file ChangeLog in this directory.
Here are the highlights:

* Snit widgets and widget adaptors now support the Tk option database.

* It's possible set the hull type of a Snit widget to be either a
  frame or a toplevel.

* It's possible to explicitly set the widget class of a Snit widget.

* It's possible to explicitly set the resource and class names for
  all locally defined and explicitly delegated options.

* Option and method names can be excluded from "delegate option *" by
  using the "except" clause, e.g.,

     delegate option * to hull except {-borderwidth -background}

* Any Snit type or widget can define a "type constructor": a body of
  code that's executed when the type is defined.  The type constructor
  is typically used to initialize array-valued type variables, and to
  add values to the Tk option database.

* Components should generally be created and installed using the new
  "install" command.

* snit::widgetadaptor hulls should generally be created and installed
  using the new "installhull using" form of the "installhull" command.

See the Snit man page and FAQ list for more information on these new 
features.


Changes in V0.81
--------------------------------------------------------------------

* All documentation errors people e-mailed to me have been fixed.

* Bug fix: weird type names.  In Snit 0.8, type names like
  "hyphenated-name" didn't work because the type name is used as a
  namespace name, and Tcl won't parse "-" as part of a namespace name
  unless you quote it somehow.  Kudos to Michael Cleverly who both
  noticed the problem and contributed the patch.

* Bug fix: Tcl 8.4.2 incompatibility.  There was a bug in Tcl 8.4.1
  (and in earlier versions, likely) that if the Tcl command "catch"
  evaluated a block that contained an explicit "return", "catch" 
  returned 0.  The documentation evidently indicated that it should
  return 2, and so this was fixed in Tcl 8.4.2.  This broke a bit
  of code in Snit.

Changes in V0.8
--------------------------------------------------------------------

* Note that there are many incompatibilities between Snit V0.8 and
  earlier versions; they are all included in this list.

* Bug fix: In Snit 0.71 and Snit 0.72, if two instances of a
  snit::type are created with the same name, the first instance's
  private data is not destroyed.  Hence, [$type info instances] will
  report that the first instance still exists.  This is now fixed.

* Snit now requires Tcl 8.4, as it depends on the new command
  tracing facility.

* The snit::widgettype command, which was previously deprecated, has
  now been deleted.

* The snit::widget command has been renamed snit::widgetadaptor; its
  usage is unchanged, except that the idiom "component hull is ..." 
  is no longer used to define the hull component.  Instead, use the
  "installhull" command:

        constructor {args} {
            installhull [label $win ...]
            $self configurelist $args
        }

* The "component" command is now obsolete, and has been removed.
  Instead, the "delegate" command implicitly defines an instance
  variable for the named component; the constructor should assign an
  object name to that instance variable.  For example, whereas you
  used to write this: 

    snit::type dog {
        delegate method wag to tail

        constructor {args} {
            component tail is [tail $self.tail -partof self]
        }

        method gettail {} {
            return [component tail]
        }
    }

  you now write this:

    snit::type dog {
        delegate method wag to tail

        constructor {args} {
            set tail [tail $self.tail -partof self]
        }

        method gettail {} {
            return $tail
        }
    }

* There is a new snit::widget command; unlike snit::widgetadaptor,
  snit::widget automatically creates a Tk frame widget as the hull
  widget; the constructor doesn't need to create and set a hull component.

* Snit objects may now be renamed without breaking; many of the
  specific changes which follow are related to this.  However,
  there are some new practices for type authors to follow if they wish
  to write renameable types and widgets.  In particular,

  * In an instance method, $self will always contain the object's
    current name, so instance methods can go on calling other instance
    methods using $self.

  * If the object is renamed, then $self's value will change.  Therefore, 
    don't use $self for anything that will break if $self changes.
    For example, don't pass a callback as "[list $self methodname]".

  * If the object passes "[list $self methodname arg1 arg2]" as a callback, 
    the callback will fail when the object is renamed.  Instead, the 
    object should pass "[mymethod methodname arg1 arg2]".  The [mymethod]
    command returns the desired command as a list beginning with a
    name for the object that never changes.

    For example, in Snit V0.71 you might have used this code to call a
    method when a Tk button is pushed: 

     .btn configure -command [list $self buttonpress]

    This still works in V0.8--but the callback will break if your
    instance is renamed.  Here's the safe way to do it:

     .btn configure -command [mymethod buttonpress]

  * Every object has a private namespace; the name of this namespace
    is now available in method bodies, etc., as "$selfns".  This value is
    constant for the life the object.  Use "$selfns" instead of "$self" if
    you need a unique token to identify the object.

  * When a snit::widget's instance command is renamed, its Tk window
    name remains the same--and is still extremely important.
    Consequently, the Tk window name is now available in snit::widget
    method bodies, etc., as "$win".  This value is constant for the
    life of the object.  When creating child windows, it's best to 
    use "$win.child" rather than "$self.child" as the name of the
    child window. 

* The names "selfns" and "win" may no longer be used as explicit argument
  names for typemethods, methods, constructors, or onconfigure
  handlers.

* procs defined in a Snit type or widget definition used to be able to
  reference instance variables if "$self" was passed to them
  explicitly as the argument "self"; this is no longer the case.

* procs defined in a Snit type or widget definition can now reference
  instance variables if "$selfns" is passed to them explicitly as the
  argument "selfns".  However, this usage is deprecated.

* All Snit type and widget instances can be destroyed by renaming the
  instance command to "".

Changes in V0.72
--------------------------------------------------------------------

* Updated the pkgIndex.tcl file to references snit 0.72 instead of
  snit 0.7.

* Fixed a bug in widget destruction that caused errors like
  "can't rename "::hull1.f": command doesn't exist".

Changes in V0.71
--------------------------------------------------------------------

* KNOWN BUG: The V0.7 documentation implies that a snit::widget can
  serve as the hull of another snit::widget.  Unfortunately, it
  doesn't work.  The fix for this turns out to be extremely
  complicated, so I plan to fix it in Snit V0.8.

  Note that a snit::widget can still be composed of other
  snit::widgets;  it's only a problem when the hull component in
  particular is a snit::widget.

* KNOWN BUG: If you rename a Snit type or instance command (i.e., using
  Tcl's [rename] command) it will no longer work properly.  This is
  part of the reason for the previous bug, and should also be fixed in
  Snit V0.8.

* Enhancement: Snit now preserves the call stack (i.e., the
  "errorInfo") when rethrowing errors thrown by Snit methods,
  typemethods, and so forth.  This should make debugging Snit types
  and widgets much easier.  In Snit V0.8, I hope to clean up the
  call stack so that Snit internals are hidden.

* Bug fix: Option default values were being processed incorrectly.  In
  particular, if the default value contained brackets, it was treated
  as a command interpolation.  For example,

    option -regexp {[a-z]+}

  yield the error that "a-z" isn't a known command.  Credit to Keith
  Waclena for finding this one.

* Bug fix: the [$type info instances] command failed to find
  instances that weren't defined in the global namespace, and found
  some things that weren't instances.  Credit to Keith Waclena for
  finding this one as well.

* Internal Change: the naming convention for instance namespaces
  within the type namespace has changed.  But then, your code
  shouldn't have depended on that anyway.

* Bug fix: snit::widget destruction was seriously broken if the hull
  component was itself a megawidget (e.g., a BWidget).
  Each layer of megawidget code needs its opportunity
  to clean up properly, and that wasn't happening.  In addition, the
  snit::widget destruction code was bound as follows:

    bind $widgetName <Destroy> {....}

  which means that if the user of a Snit widget needs to bind to
  <Destroy> on the widget name they've just wiped out Snit's
  destructor.  Consequently, Snit now creates a bindtag called
  
    Snit<widgettype>

  e.g.,

    Snit::rotext

  and binds its destroy handler to that.  This bindtag is inserted in
  the snit::widget's bindtags immediately after the widget name.

  Destruction is always going to be somewhat tricky when multiple
  levels of megawidgets are involved, as you need to make sure that
  the destructors are called in inverse order of creation.

Changes in V0.7
----------------------------------------------------------------------

* INCOMPATIBILITY: Snit constructor definitions can now have arbitrary
  argument lists, as methods do.  That is, the type's create method 
  expects the instance name followed by exactly the arguments defined
  in the constructor's argument list: 

    snit::type dog {
        variable data
        constructor {breed color} {
            set data(breed) $breed
            set data(color) $color
        }
    }

    dog spot labrador chocolate

  To get the V0.6 behavior, use the argument "args".  That is, the
  default constructor would be defined in this way:

    snit::type dog {
        constructor {args} {
            $self configurelist $args
        }
    }

* Added a "$type destroy" type method.  It destroys all instances of
  the type properly (if possible) then deletes the type's namespace
  and type command.

Changes in V0.6
-----------------------------------------------------------------

* Minor corrections to the man page.

* The command snit::widgettype is deprecated, in favor of
  snit::widget.

* The variable "type" is now automatically defined in all methods,
  constructors, destructors, typemethods, onconfigure handlers, and
  oncget handlers.  Thus, a method can call type methods as "$type
  methodname".

* The new standard instance method "info" is used for introspection on 
  type and widget instances:

  $object info type
     Returns the object's type.

  $object info vars
     Returns a list of the object's instance variables (excluding Snit
     internal variables).  The names are fully qualified.

  $object info typevars
     Returns a list of the object's type's type variables (excluding
     Snit internal variables).  The names are fully qualified.

  $object info options
     Returns a list of the object's option names.  This always
     includes local options and explicitly delegated options.  If
     unknown options are delegated as well, and if the component to
     which they are delegated responds to "$object configure" like Tk
     widgets do, then the result will include all possible unknown
     options which could be delegated to the component.  

     Note that the return value might be different for different
     instances of the same type, if component object types can vary
     from one instance to another.

* The new standard typemethod "info" is used for introspection on
  types:

  $type info typevars
     Returns a list of the type's type variables (excluding Snit
     internal variables).

  $type info instances
     Returns a list of the instances of the type.  For non-widget
     types, each instance will be the fully-qualified instance command
     name; for widget types, each instance will be a widget name.

* Bug fixed: great confusion resulted if the hull component of a
  snit::widgettype was another snit::widgettype.  Snit takes over the
  hull widget's Tk widget command by renaming it to a known name, and
  putting its own command in its place.  The code made no allowance
  for the fact that this might happen more than once; the second time,
  the original Tk widget command would be lost.  Snit now ensures that
  the renamed widget command is given a unique name.

* Previously, instance methods could call typemethods by name, as
  though they were normal procs.  The downside to this was that
  if a typemethod name was the same as a standard Tcl command, the
  typemethod shadowed the standard command in all of the object's
  code.  This is extremely annoying should you wish to define a
  typemethod called "set".  Instance methods must now call typemethods
  using the type's command, as in "$type methodname".

* Typevariable declarations are no longer required in
  typemethods, methods, or procs provided that the typevariables are defined
  in the main type or widget definition.

* Instance variable declarations are no longer required in methods provided
  that the instance variables are defined in the main type or widget
  declaration.

* Instance variable declarations are no longer required in procs,
  provided that the instance variables are defined in the main type or
  widget declaration.  Any proc that includes "self" in its argument
  list will pick up all such instance variables automatically.

* The "configure" method now returns output consistent with Tk's when
  called with 0 or 1 arguments, i.e., it returns information about one
  or all options.  For options defined by Snit objects, the "dbname"
  and "classname" returned in the output will be {}.  "configure" does
  its best to do the right thing in the face of delegation.

* If the string "%AUTO%" appears in the "name" argument to "$type create"
  or "$widgettype create", it will be replaced with a string that
  looks like "$type$n", where "$type" is the type name and "$n" is 
  a counter that's incremented each time a
  widget of this type is created.  This allows the caller to create 
  effectively anonymous instances:

  widget mylabel {...}

  set w [mylabel .pane.toolbar.%AUTO% ...]
  $w configure -text "Some text"

* The "create" typemethod is now optional for ordinary types so long
  as the desired instance name is different than any typemethod name
  for that type.  Thus, the following code creates two dogs, ::spot
  and ::fido.

  type dog {...}

  dog create spot
  dog fido

  If there's a conflict between the instance name and a typemethod, 
  either use "create" explicitly, or fully qualify the instance name:

  dog info -color black           ;# Error; assumes "info" typemethod.
  dog create info -color black    ;# OK
  dog ::info -color black         ;# also OK

* Bug fix: If any Snit method, typemethod, constructor, or onconfigure
  handler defines an explicit argument called "type" or "self", the type
  definition now throws an error, preventing confusing runtime
  behavior.

* Bug fix: If a Snit type or widget definition attempts to define a
  method or option locally and also delegate it to a component, the 
  type definition now throws an error, preventing confusing runtime 
  behavior.

* Bug(?) Fix: Previously, the "$self" command couldn't be used in
  snit::widget constructors until after the hull component was
  defined.  It is now possible to use the "$self" command to call
  instance methods at any point in the snit::widget's
  constructor--always bearing in mind that it's an error to configure
  delegated options or are call delegated methods before creating the
  component to which they are delegated.

Changes in V0.5
------------------------------------------------------------------

* Updated the test suite so that Tk-related tests are only run if
  Tk is available.  Credit Jose Nazario for pointing out the problem.

* For snit::widgettypes, the "create" keyword is now optional when 
  creating a new instance.  That is, either of the following will
  work:

  ::snit::widgettype mylabel { }

  mylabel create .lab1 -text "Using create typemethod"
  mylabel .lab2 -text "Implied create typemethod"

  This means that snit::widgettypes can be used identically to normal
  Tk widgets.  Credit goes to Colin McCormack for suggesting this.

* Destruction code is now defined using the "destructor" keyword
  instead of by defining a "destroy" method.  If you've been 
  defining the "destroy" method, you need to replace it with 
  "destructor" immediately.  See the man page for the syntax.

* widgettype destruction is now handled properly (it was buggy).  
  Use the Tk command "destroy" to destroy instances of a widgettype;
  the "destroy" method isn't automatically defined for widgettypes as
  it is for normal types, and has no special significance even if it
  is defined.

* Added the "from" command to aid in parsing out specific option
  values in constructors.

Changes in V0.4
------------------------------------------------------------------

* Added the "codename" command, to qualify type method and private
  proc names.

* Changed the internal implementation of Snit types and widget types
  to prevent an obscure kind of error and to make it easier to pass
  private procs as callback commands to other objects.  Credit to Rolf
  Ade for discovering the hole.

Changes in V0.3
------------------------------------------------------------------

* First public release.


