
/*
    Copyright (C) 1999-2008 by Mark D. Hill and David A. Wood for the
    Wisconsin Multifacet Project.  Contact: gems@cs.wisc.edu
    http://www.cs.wisc.edu/gems/

    --------------------------------------------------------------------

    This file is part of the Ruby Multiprocessor Memory System Simulator, 
    a component of the Multifacet GEMS (General Execution-driven 
    Multiprocessor Simulator) software toolset originally developed at 
    the University of Wisconsin-Madison.

    Ruby was originally developed primarily by Milo Martin and Daniel
    Sorin with contributions from Ross Dickson, Carl Mauer, and Manoj
    Plakal.

    Substantial further development of Multifacet GEMS at the
    University of Wisconsin was performed by Alaa Alameldeen, Brad
    Beckmann, Jayaram Bobba, Ross Dickson, Dan Gibson, Pacia Harper,
    Derek Hower, Milo Martin, Michael Marty, Carl Mauer, Michelle Moravan,
    Kevin Moore, Andrew Phelps, Manoj Plakal, Daniel Sorin, Haris Volos, 
    Min Xu, and Luke Yen.
    --------------------------------------------------------------------

    If your use of this software contributes to a published paper, we
    request that you (1) cite our summary paper that appears on our
    website (http://www.cs.wisc.edu/gems/) and (2) e-mail a citation
    for your published paper to gems@cs.wisc.edu.

    If you redistribute derivatives of this software, we request that
    you notify us and either (1) ask people to register with us at our
    website (http://www.cs.wisc.edu/gems/) or (2) collect registration
    information and periodically send it to us.

    --------------------------------------------------------------------

    Multifacet GEMS is free software; you can redistribute it and/or
    modify it under the terms of version 2 of the GNU General Public
    License as published by the Free Software Foundation.

    Multifacet GEMS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with the Multifacet GEMS; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
    02111-1307, USA

    The GNU General Public License is contained in the file LICENSE.

### END HEADER ###
*/

/*
 * $Id$
 * 
 * Description: This is the virtual base class of all classes that can
 * be the targets of wakeup events.  There is only two methods,
 * wakeup() and print() and no data members.
 *
 */

#ifndef CONSUMER_H
#define CONSUMER_H

//#include "Global.h"
#include "EventQueue.h"
#include "NetworkDefine.h"
#include "Common.h"
class MessageBuffer;

class Consumer {
public:
  // Constructors
  Consumer() { m_last_scheduled_wakeup = 0; m_last_wakeup = 0; m_out_link_vec.setSize(0); }

  // Destructor
  virtual ~Consumer() { }
  
  // Public Methods - pure virtual methods
  void triggerWakeup() { Time time = g_eventQueue_ptr->getTime(); if (m_last_wakeup != time) { wakeup(); m_last_wakeup = time; }}
  virtual void wakeup() = 0;
  virtual void print(ostream& out) const = 0;
  const Time& getLastScheduledWakeup() const { return m_last_scheduled_wakeup; }
  void setLastScheduledWakeup(const Time& time) { m_last_scheduled_wakeup = time; }
  Vector< Vector<MessageBuffer*> > getOutBuffers() { return m_out_link_vec; }

protected:
  Vector< Vector<MessageBuffer*> > m_out_link_vec;  

private:
  // Private Methods
  
  // Data Members (m_ prefix)
  Time m_last_scheduled_wakeup;
  Time m_last_wakeup;
};

// Output operator declaration
inline extern 
ostream& operator<<(ostream& out, const Consumer& obj);

// ******************* Definitions *******************

// Output operator definition
inline extern 
ostream& operator<<(ostream& out, const Consumer& obj)
{
  obj.print(out);
  out << flush;
  return out;
}

#endif //CONSUMER_H
