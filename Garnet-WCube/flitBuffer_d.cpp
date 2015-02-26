/*
    Copyright (C) 1999-2008 by Mark D. Hill and David A. Wood for the
    Wisconsin Multifacet Project.  Contact: gems@cs.wisc.edu
    http://www.cs.wisc.edu/gems/

    --------------------------------------------------------------------

    This file is part of Garnet (Princeton's interconnect model),
    a component of the Multifacet GEMS (General Execution-driven 
    Multiprocessor Simulator) software toolset originally developed at 
    the University of Wisconsin-Madison.

    Garnet was developed by Niket Agarwal at Princeton University. Orion was
    developed by Princeton University.

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
 * flitBuffer_d.C
 *
 * Niket Agarwal, Princeton University
 *
 * */

#include "flitBuffer_d.h"

flitBuffer_d::flitBuffer_d()
{
	max_size = INFINITE_;
}

flitBuffer_d::flitBuffer_d(int maximum_size)
{
	max_size = maximum_size;
}

bool flitBuffer_d::isEmpty()
{
	return (m_buffer.size() == 0);
}

bool flitBuffer_d::isReady()
{ 
	if(m_buffer.size() != 0 )
	{
		flit_d *t_flit = m_buffer.peekMin();
		if(t_flit->get_time() <=  g_eventQueue_ptr->getTime())
			return true;
	}
	return false;	
}

bool flitBuffer_d::isReadyForNext()
{ 
	if(m_buffer.size() != 0 )
	{
		flit_d *t_flit = m_buffer.peekMin();
		if(t_flit->get_time() <= (g_eventQueue_ptr->getTime() + 1))
			return true;
	}
	return false;	
}

void flitBuffer_d::print(ostream& out) const
{
	out << "[flitBuffer: ";
	out << m_buffer.size() << "] " << endl;
}

bool flitBuffer_d::isFull() 
{
	return (m_buffer.size() >= max_size);
}
void flitBuffer_d::setMaxSize(int maximum)
{
	max_size = maximum;
}
