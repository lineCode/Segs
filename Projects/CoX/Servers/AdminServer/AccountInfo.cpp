/*
 * Super Entity Game Server Project
 * http://segs.sf.net/
 * Copyright (c) 2006 - 2016 Super Entity Game Server Team (see Authors.txt)
 * This software is licensed! (See License.txt for details)
 *
 */

// segs includes
#include "AdminServer.h"
#include "CharacterDatabase.h"
#include "Database.h"
#include "AccountInfo.h"
#include "Entity.h"
#include "Character.h"
#include "Costume.h"

#include <sstream>

Character * AccountInfo::get_character(size_t idx)
{
    assert(idx<m_characters.size());
    return m_characters[idx];
}

void AccountInfo::reset()
{
    for(size_t i=0; i<m_characters.size(); i++)
    {
        delete m_characters[i];
        m_characters[i]=nullptr;
    }
}

AccountInfo::AccountInfo() : m_game_server_acc_id(0),m_max_slots(0)
{

}

Character * AccountInfo::create_new_character()
{
    for(size_t i=0; i<m_characters.size(); i++)
    {
        if(m_characters[i]->getName().compare("EMPTY")==0)
            return m_characters[i]; // free slot
    }
    return nullptr;
}
int8_t AccountInfo::char_slot_index(Character *c)
{
    for(size_t i=0; i<m_characters.size(); i++)
    {
        if(m_characters[i]==c)
            return int8_t(i);
    }
    return -1;
}
bool AccountInfo::store_new_character(Entity *e, Character *character)
{
    int8_t slot_idx=char_slot_index(character);
    if(slot_idx==-1)
        return false;
    CharacterDatabase *cdb = AdminServer::instance()->character_db();
    DbTransactionGuard grd(*cdb->getDb());
    if(false==cdb->create(m_game_server_acc_id,slot_idx,e))
        return false;
    grd.commit();
    return true;
}
