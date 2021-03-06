#include "stdafx.h"
#include "UIInventoryWnd.h"
#include "../level.h"
#include "../actor.h"
#include "../ActorCondition.h"
#include "../hudmanager.h"
#include "../inventory.h"
#include "UIInventoryUtilities.h"
#include "UIPropertiesBox.h"
#include "UICellItem.h"
#include "UICellItemFactory.h"
#include "UIDragDropListEx.h"
#include "UI3tButton.h"

#include "../pch_script.h"
#include "../game_object_space.h"
#include "../script_callback_ex.h"
#include "../script_game_object.h"
#include "../Actor.h"
#include "../Actor_Flags.h"

#include "UIItemInfo.h"

#include "../../../build_config_defines.h"

#include "../CustomOutfit.h"
#include "../Helmet.h"

CUICellItem* CUIInventoryWnd::CurrentItem()
{
	return m_pCurrentCellItem;
}

PIItem CUIInventoryWnd::CurrentIItem()
{
	return	(m_pCurrentCellItem)?(PIItem)m_pCurrentCellItem->m_pData : NULL;
}

void CUIInventoryWnd::SetCurrentItem(CUICellItem* itm)
{
	if(m_pCurrentCellItem == itm) return;
	m_pCurrentCellItem				= itm;
}

void CUIInventoryWnd::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	if(pWnd == &UIPropertiesBox &&	msg==PROPERTY_CLICKED)
	{
		ProcessPropertiesBoxClicked	();
	}else 
	if (UIExitButton == pWnd && BUTTON_CLICKED == msg)
	{
		GetHolder()->StartStopMenu			(this,true);
	}

	CUIWindow::SendMessage(pWnd, msg, pData);
}


void CUIInventoryWnd::InitInventory_delayed()
{
	m_b_need_reinit = true;
}

void CUIInventoryWnd::InitInventory() 
{
	CInventoryOwner *pInvOwner	= smart_cast<CInventoryOwner*>(Level().CurrentEntity());
	if(!pInvOwner)				return;

	m_pInv						= &pInvOwner->inventory();

	UIPropertiesBox.Hide		();

	UIActorProtectionInfo->Show(false);

	InfoCurItem(NULL);
	
	clear_highlight_lists       ();
	ClearAllLists				();
	m_pMouseCapturer			= NULL;
	SetCurrentItem				(NULL);

	UpdateButtonsLayout();

	PIItem  _itm							= m_pInv->m_slots[PISTOL_SLOT].m_pIItem;
	if(_itm)
	{
		CUICellItem* itm				= create_cell_item(_itm);
		m_pUIPistolList->SetItem		(itm);
	}
	_itm								= m_pInv->m_slots[RIFLE_SLOT].m_pIItem;
	if(_itm)
	{
		CUICellItem* itm				= create_cell_item(_itm);
		m_pUIAutomaticList->SetItem		(itm);
	}
	_itm								= m_pInv->m_slots[KNIFE_SLOT].m_pIItem;
	if(_itm)
	{
		CUICellItem* itm				= create_cell_item(_itm);
		m_pUIKnifeList->SetItem		(itm);
	}
	_itm								= m_pInv->m_slots[APPARATUS_SLOT].m_pIItem;
	if(_itm)
	{
		CUICellItem* itm				= create_cell_item(_itm);
		m_pUIBinocularList->SetItem		(itm);
	}
	_itm								= m_pInv->m_slots[DETECTOR_ONE_SLOT].m_pIItem;
	if(_itm)
	{
		CUICellItem* itm				= create_cell_item(_itm);
		m_pUIDetectorOneList->SetItem		(itm);
	}
	_itm								= m_pInv->m_slots[TORCH_SLOT].m_pIItem;
	if(_itm)
	{
		CUICellItem* itm				= create_cell_item(_itm);
		m_pUITorchList->SetItem		(itm);
	}
	_itm								= m_pInv->m_slots[PDA_SLOT].m_pIItem;
	if(_itm)
	{
		CUICellItem* itm				= create_cell_item(_itm);
		m_pUIPDAList->SetItem		(itm);
	}
	_itm								= m_pInv->m_slots[HELMET_SLOT].m_pIItem;
	if(_itm)
	{
		CUICellItem* itm				= create_cell_item(_itm);
		m_pUIHelmetList->SetItem		(itm);
	}
	_itm								= m_pInv->m_slots[SLOT_QUICK_ACCESS_0].m_pIItem;
	if(_itm)
	{
		CUICellItem* itm				= create_cell_item(_itm);
		m_pUISlotQuickAccessList_0->SetItem		(itm);
	}
	_itm								= m_pInv->m_slots[SLOT_QUICK_ACCESS_1].m_pIItem;
	if(_itm)
	{
		CUICellItem* itm				= create_cell_item(_itm);
		m_pUISlotQuickAccessList_1->SetItem		(itm);
	}
	_itm								= m_pInv->m_slots[SLOT_QUICK_ACCESS_2].m_pIItem;
	if(_itm)
	{
		CUICellItem* itm				= create_cell_item(_itm);
		m_pUISlotQuickAccessList_2->SetItem		(itm);
	}
	_itm								= m_pInv->m_slots[SLOT_QUICK_ACCESS_3].m_pIItem;
	if(_itm)
	{
		CUICellItem* itm				= create_cell_item(_itm);
		m_pUISlotQuickAccessList_3->SetItem		(itm);
	}
	_itm								= m_pInv->m_slots[NIGHTVISION_SLOT].m_pIItem;
	if(_itm)
	{
		CUICellItem* itm				= create_cell_item(_itm);
		m_pUINightvisionList->SetItem		(itm);
	}
	_itm								= m_pInv->m_slots[DETECTOR_TWO_SLOT].m_pIItem;
	if(_itm)
	{
		CUICellItem* itm				= create_cell_item(_itm);
		m_pUIDetectorTwoList->SetItem		(itm);
	}
	_itm								= m_pInv->m_slots[OUTFIT_SLOT].m_pIItem;
	if(_itm)
	{
		CUICellItem* itm				= create_cell_item(_itm);
		m_pUIOutfitList->SetItem		(itm);
	}

    TIItemContainer::iterator itb = m_pInv->m_belt.begin();
    TIItemContainer::iterator ite = m_pInv->m_belt.end();
    for (; itb != ite; ++itb)
    {
        CUICellItem* itm = create_cell_item(*itb);
        m_pUIBeltList->SetItem(itm);
    }

	TIItemContainer ruck_list;
    ruck_list = m_pInv->m_ruck;
    std::sort(ruck_list.begin(), ruck_list.end(), InventoryUtilities::GreaterRoomInRuck);

	itb = ruck_list.begin();
    ite = ruck_list.end();

	for (; itb != ite; ++itb)
	{
		CUICellItem* itm			= create_cell_item(*itb);
		m_pUIBagList->SetItem		(itm);
	}

	if (psActorFlags.test(AF_ACTOR_PROTECTION_INFO_ENABLE))
		UIActorProtectionInfo->Show(true);
	
	InventoryUtilities::UpdateWeight					(UIBagWnd, true);

	m_b_need_reinit					= false;
}

void CUIInventoryWnd::DropCurrentItem(bool b_all)
{

	CActor *pActor			= smart_cast<CActor*>(Level().CurrentEntity());
	if(!pActor)				return;

	if(!b_all && CurrentIItem() && !CurrentIItem()->IsQuestItem())
	{
		SendEvent_Item_Drop		(CurrentIItem());
		SetCurrentItem			(NULL);
		InventoryUtilities::UpdateWeight			(UIBagWnd, true);
		return;
	}

	if(b_all && CurrentIItem() && !CurrentIItem()->IsQuestItem())
	{
		u32 cnt = CurrentItem()->ChildsCount();

		for(u32 i=0; i<cnt; ++i){
			CUICellItem*	itm				= CurrentItem()->PopChild();
			PIItem			iitm			= (PIItem)itm->m_pData;
			SendEvent_Item_Drop				(iitm);
		}

		SendEvent_Item_Drop					(CurrentIItem());
		SetCurrentItem						(NULL);
		InventoryUtilities::UpdateWeight	(UIBagWnd, true);
		return;
	}
}

//------------------------------------------

bool CUIInventoryWnd::ToSlot(CUICellItem* itm, bool force_place)
{
	CUIDragDropListEx*	old_owner			= itm->OwnerList();
	PIItem	iitem							= (PIItem)itm->m_pData;
	u32 _slot								= iitem->GetSlot();
		

	if(GetInventory()->CanPutInSlot(iitem)){		
		

		CUIDragDropListEx* new_owner		= GetSlotList(_slot);
		
		if(_slot==GRENADE_SLOT && !new_owner )
#if defined(GRENADE_FROM_BELT)
			return false;
#else
			return true; //fake, sorry (((
#endif
		


	 #if defined(INV_MOVE_ITM_INTO_QUICK_SLOTS) 
			if ((_slot == SLOT_QUICK_ACCESS_0)||(_slot == SLOT_QUICK_ACCESS_1)||(_slot == SLOT_QUICK_ACCESS_2)||(_slot == SLOT_QUICK_ACCESS_3)){
				for(u32 i=SLOT_QUICK_ACCESS_0; i <= SLOT_QUICK_ACCESS_3; ++i ) 
				{	
					if(i != _slot){
						 PIItem l_pIItem = GetInventory()->m_slots[i].m_pIItem;
						 if(l_pIItem){
							if ((!xr_strcmp(l_pIItem->object().cNameSect(), iitem->object().cNameSect()))&&(l_pIItem != iitem)){
								PIItem	_iitem						= GetInventory()->m_slots[i].m_pIItem;
								CUIDragDropListEx* slot_list		= GetSlotList(i);
								VERIFY								(slot_list->ItemsCount()==1);
								CUICellItem* slot_cell				= slot_list->GetItemIdx(0);
								VERIFY								(slot_cell && ((PIItem)slot_cell->m_pData)==_iitem);
								bool result							= ToBag(slot_cell, false);
								VERIFY								(result);
								}
							 }
						}
					}
				}
	#endif	


		bool result							= GetInventory()->Slot(iitem);
		if (!new_owner)
		{
			Msg("!ERROR: Bad slot %d", _slot);
			GetSlotList(_slot);  // for tracing
			return false;
		}
	

		VERIFY								(result);
#ifdef DEBUG_SLOTS
		Msg("# inventory wnd ToSlot (0x%p) from old_owner = 0x%p ", itm, old_owner);
#endif
		CUICellItem* i						= old_owner->RemoveItem(itm, (old_owner==new_owner) );
		

		new_owner->SetItem					(i);

		// Real Wolf: ���� �� ���� ��� ������� ������� Slot. 16.01.14
		//SendEvent_Item2Slot					(iitem);

	#if defined(INV_NO_ACTIVATE_APPARATUS_SLOT)
		if (activate_slot(_slot))
			SendEvent_ActivateSlot				(iitem);
	#else
		SendEvent_ActivateSlot				(iitem);
	#endif

		/************************************************** added by Ray Twitty (aka Shadows) START **************************************************/
		// ��������� ������ ���� � ���������
		InventoryUtilities::UpdateWeight	(UIBagWnd, true);
		/*************************************************** added by Ray Twitty (aka Shadows) END ***************************************************/
		m_b_need_reinit = true;
		
		return								true;
	}else
	{ // in case slot is busy
		if(!force_place ||  _slot==NO_ACTIVE_SLOT || GetInventory()->m_slots[_slot].m_bPersistent) return false;

		PIItem	_iitem						= GetInventory()->m_slots[_slot].m_pIItem;
		CUIDragDropListEx* slot_list		= GetSlotList(_slot);

		if (0 == slot_list->ItemsCount())
		{
			Msg("!ERROR: slot(%d).ItemsCount = 0 (no cells) ", _slot);
			return false;
		}



		VERIFY								(slot_list->ItemsCount()==1);
		VERIFY								(slot_list->ItemsCount()>=1);


		CUICellItem* slot_cell				= slot_list->GetItemIdx(0);
		VERIFY								(slot_cell && ((PIItem)slot_cell->m_pData)==_iitem);

		bool result							= ToBag(slot_cell, false);
		VERIFY								(result);

		return ToSlot						(itm, false);
	}
}

bool CUIInventoryWnd::ToBag(CUICellItem* itm, bool b_use_cursor_pos)
{
	PIItem	iitem						= (PIItem)itm->m_pData;

	if(GetInventory()->CanPutInRuck(iitem))
	{
#ifdef DEBUG_SLOTS
		Msg("# inventory wnd ToBag (0x%p) ", itm);
#endif

		CUIDragDropListEx*	old_owner		= itm->OwnerList();
		CUIDragDropListEx*	new_owner		= NULL;
		if(b_use_cursor_pos){
				new_owner					= CUIDragDropListEx::m_drag_item->BackList();
				VERIFY						(new_owner==m_pUIBagList);
		}else
				new_owner					= m_pUIBagList;


		bool result							= GetInventory()->Ruck(iitem);
		VERIFY								(result);
		CUICellItem* i						= old_owner->RemoveItem(itm, (old_owner==new_owner) );

		/************************************************** added by Ray Twitty (aka Shadows) START **************************************************/
		// ��������� ������ ���� � ���������
		InventoryUtilities::UpdateWeight	(UIBagWnd, true);
		/*************************************************** added by Ray Twitty (aka Shadows) END ***************************************************/

#ifdef INV_RUCK_UNLIMITED_FIX		
		if (result = new_owner->CanSetItem(i) || new_owner->IsAutoGrow() )
		{
#endif
			if(b_use_cursor_pos)
				new_owner->SetItem				(i,old_owner->GetDragItemPosition() );
			else
				new_owner->SetItem				(i);
			SendEvent_Item2Ruck					(iitem);
#ifdef INV_RUCK_UNLIMITED_FIX
		}
		else
		{
			NET_Packet					P;
			iitem->object().u_EventGen	(P, GE_OWNERSHIP_REJECT, iitem->object().H_Parent()->ID());
			P.w_u16						(u16(iitem->object().ID()));
			iitem->object().u_EventSend(P);
		}

		m_b_need_reinit = true;
		
		return result;
#else
		m_b_need_reinit = true;
		
		return true;
#endif
	}
	return false;
}

bool CUIInventoryWnd::ToBelt(CUICellItem* itm, bool b_use_cursor_pos)
{
	PIItem	iitem						= (PIItem)itm->m_pData;

	if(GetInventory()->CanPutInBelt(iitem))
	{
		CUIDragDropListEx*	old_owner		= itm->OwnerList();
		CUIDragDropListEx*	new_owner		= NULL;
		if(b_use_cursor_pos){
				new_owner					= CUIDragDropListEx::m_drag_item->BackList();
				VERIFY						(new_owner==m_pUIBeltList);
		}else
				new_owner					= m_pUIBeltList;

		bool result							= GetInventory()->Belt(iitem);
		VERIFY								(result);
		CUICellItem* i						= old_owner->RemoveItem(itm, (old_owner==new_owner) );
		
	//.	UIBeltList.RearrangeItems();
		if(b_use_cursor_pos)
			new_owner->SetItem				(i,old_owner->GetDragItemPosition());
		else
			new_owner->SetItem				(i);

		SendEvent_Item2Belt					(iitem);

		/************************************************** added by Ray Twitty (aka Shadows) START **************************************************/
		// ��������� ������ ���� � ���������
		InventoryUtilities::UpdateWeight	(UIBagWnd, true);
		/*************************************************** added by Ray Twitty (aka Shadows) END ***************************************************/
		m_b_need_reinit = true;
		
		return true;
	}
	return									false;
}

void CUIInventoryWnd::AddItemToBag(PIItem pItem)
{
	CUICellItem* itm						= create_cell_item(pItem);
	m_pUIBagList->SetItem					(itm);
}

bool CUIInventoryWnd::OnItemStartDrag(CUICellItem* itm)
{
	InfoCurItem(NULL);
	return false; //default behaviour
}

bool CUIInventoryWnd::OnItemSelected(CUICellItem* itm)
{
	SetCurrentItem		(itm);
	InfoCurItem(NULL);
	m_item_info_view = false;
	return				false;
}

bool CUIInventoryWnd::OnItemFocusReceive(CUICellItem* itm)
{
    InfoCurItem(NULL);
	m_item_info_view = true;

	itm->m_selected = true;
	set_highlight_item(itm);
    return true;
}

bool CUIInventoryWnd::OnItemFocusLost(CUICellItem* itm)
{
	if (itm)
    {
        itm->m_selected = false;
    }
	InfoCurItem(NULL);
	clear_highlight_lists();
    return true;
}

bool CUIInventoryWnd::OnItemFocusedUpdate(CUICellItem* itm)
{
if (itm)
    {
        itm->m_selected = true;
        if (m_highlight_clear)
        {
            set_highlight_item(itm);
        }
    }
    VERIFY(m_ItemInfo);
    if (Device.dwTimeGlobal < itm->FocusReceiveTime() + m_ItemInfo->delay)
    {
        return true; // false
    }
    if (CUIDragDropListEx::m_drag_item || UIPropertiesBox.IsShown() || !m_item_info_view)
    {
        return true;
    }

    InfoCurItem(itm);
    return true;
}

bool CUIInventoryWnd::OnItemDrop(CUICellItem* itm)
{
	InfoCurItem(NULL);
	CUIDragDropListEx*	old_owner		= itm->OwnerList();
	CUIDragDropListEx*	new_owner		= CUIDragDropListEx::m_drag_item->BackList();
	if(old_owner==new_owner || !old_owner || !new_owner)
					return false;

	EListType t_new		= GetType(new_owner);
	EListType t_old		= GetType(old_owner);

	if(t_new == t_old && t_new != iwSlot) return true;
	 switch(t_new){
	
	 case iwSlot:
	{
		PIItem item = CurrentIItem();
		bool can_put = false;
		Ivector2 max_size = new_owner->CellSize();

		LPCSTR name = item->object().Name_script();
		int item_w = item->GetGridWidth();
		int item_h = item->GetGridHeight();

		if (new_owner->GetVerticalPlacement())
			std::swap(max_size.x, max_size.y); 

		if (item_w <= max_size.x && item_h <= max_size.y)
		{
			for (u32 i = 0; i < SLOTS_TOTAL; i++)
				if (new_owner == m_slots_array[i])
				{				
					if (item->IsPlaceable(i, i))
					{
						item->SetSlot(i);
						can_put = true;
						Msg("# Drag-drop item %s to slot %d accepted ", name, i);
					}
					else
					{
						string256 tmp = { 0 };
						for (u32 ii = 0; ii < item->GetSlotsCount(); ii++)
						{
							u32 len = xr_strlen(tmp);
							u32 slot = item->GetSlots()[ii];
							sprintf_s(&tmp[len], 256 - len, "%d ", slot);
						}
						Msg("!WARN: cannot put item %s into slot %d, allowed slots { %s}", name, i, tmp);
					}
					break;
				}   // for-if 
		}
		else
			Msg("!#ERROR: item %s to large for slot: (%d x %d) vs (%d x %d) ", name, item_w, item_h, max_size.x, max_size.y );

			
		if(!can_put) 
			break; // �� �������� � ����, ������ �������� �� ����� ���������?
					
			if(GetSlotList(item->GetSlot()) == new_owner)
				ToSlot	(itm, true);
		}break;
		case iwBag:{
			ToBag	(itm, true);
		}break;
		case iwBelt:{
			ToBelt	(itm, true);
		}break;
	};

	DropItem				(CurrentIItem(), new_owner);

	return true;
}

#include "../eatable_item.h"
bool CUIInventoryWnd::OnItemDbClick(CUICellItem* itm)
{
	InfoCurItem(NULL);
	PIItem	__item		= (PIItem)itm->m_pData;
	u32		__slot		= __item->GetSlot();
	auto	old_owner	= itm->OwnerList();
	if (__slot < SLOT_QUICK_ACCESS_0 || __slot > SLOT_QUICK_ACCESS_3){
		if(TryUseItem(__item))
		return true;
	}else{
		PIItem iitem = GetInventory()->Same(__item,true);
		if(iitem){
			if(TryUseItem(iitem))
			return true;
		}else{
			if(TryUseItem(__item))
			return true;
		}	
	}

	
	EListType t_old						= GetType(old_owner);

	switch(t_old){
		case iwSlot:{
			ToBag	(itm, false);
		}break;

		case iwBag:
		{
			// �������� ����� ��������� ���� �� ������ �����������.
			// ���� ��� ����, �� ������������� ������ ������ ����, ��������� � ������.
			auto slots = __item->GetSlots();
			for (u8 i = 0; i < (u8)slots.size(); ++i)
			{
					__item->SetSlot(slots[i]);
					if (ToSlot(itm, false) )
						return true;		
			}			
			__item->SetSlot(slots.size()? slots[0]: NO_ACTIVE_SLOT);
			if(!ToSlot(itm, false))
			{
				if( !ToBelt(itm, false) )
					ToSlot	(itm, true);
			}
		}break;

		case iwBelt:{
			ToBag	(itm, false);
		}break;
	};

	return true;
}


bool CUIInventoryWnd::OnItemRButtonClick(CUICellItem* itm)
{
	SetCurrentItem				(itm);
	InfoCurItem(NULL);
	m_item_info_view = true;
	ActivatePropertiesBox		();
	return						false;
}

CUIDragDropListEx* CUIInventoryWnd::GetSlotList(u32 slot_idx)
{	
	if(slot_idx == NO_ACTIVE_SLOT || GetInventory()->m_slots[slot_idx].m_bPersistent)	return NULL;
	return m_slots_array[slot_idx];	
}

void CUIInventoryWnd::ClearAllLists()
{
	m_pUIBagList->ClearAll					(true);
	m_pUIBeltList->ClearAll					(true);
	m_pUIOutfitList->ClearAll				(true);
	m_pUIPistolList->ClearAll				(true);
	m_pUIAutomaticList->ClearAll			(true);
	m_pUIKnifeList->ClearAll				(true);
	m_pUIBinocularList->ClearAll			(true);
	m_pUIDetectorOneList->ClearAll			(true);
	m_pUITorchList->ClearAll				(true);
	m_pUIPDAList->ClearAll					(true);
	m_pUIHelmetList->ClearAll				(true);
	m_pUISlotQuickAccessList_0->ClearAll	(true);
	m_pUISlotQuickAccessList_1->ClearAll	(true);
	m_pUISlotQuickAccessList_2->ClearAll	(true);
	m_pUISlotQuickAccessList_3->ClearAll	(true);
	m_pUINightvisionList->ClearAll			(true);
	m_pUIDetectorTwoList->ClearAll			(true);
}

void CUIInventoryWnd::UpdateActorPotectionProgressBars()
{	
	if (!psActorFlags.test(AF_ACTOR_PROTECTION_INFO_ENABLE)) return;

	CActor*	pActor = smart_cast<CActor*>(Level().CurrentViewEntity());

	if(!pActor) return;
	
	float burn_value = 0.0f;
	float strk_value = 0.0f;
    float radi_value = 0.0f;
    float cmbn_value = 0.0f;
    float tele_value = 0.0f;
    float woun_value = 0.0f;
    float shoc_value = 0.0f;
    float fwou_value = 0.0f;
    float expl_value = 0.0f;

	CCustomOutfit* outfit = pActor->GetOutfit();
    CHelmet* helmet = pActor->GetHelmet();
	
    if (outfit) // Kondr48: ������ �����������
    {
        burn_value += outfit->GetDefHitTypeProtection(ALife::eHitTypeBurn);
		strk_value += outfit->GetDefHitTypeProtection(ALife::eHitTypeStrike);
        shoc_value += outfit->GetDefHitTypeProtection(ALife::eHitTypeShock); 
        woun_value += outfit->GetDefHitTypeProtection(ALife::eHitTypeWound);
		radi_value += outfit->GetDefHitTypeProtection(ALife::eHitTypeRadiation);
        tele_value += outfit->GetDefHitTypeProtection(ALife::eHitTypeTelepatic);
		cmbn_value += outfit->GetDefHitTypeProtection(ALife::eHitTypeChemicalBurn);
		expl_value += outfit->GetDefHitTypeProtection(ALife::eHitTypeExplosion);
		fwou_value += outfit->GetDefHitTypeProtection(ALife::eHitTypeFireWound);
	}

	if (helmet) // Kondr48: ������ �����
    {
        burn_value += helmet->GetDefHitTypeProtection(ALife::eHitTypeBurn);
		strk_value += helmet->GetDefHitTypeProtection(ALife::eHitTypeStrike);
        shoc_value += helmet->GetDefHitTypeProtection(ALife::eHitTypeShock); 
        woun_value += helmet->GetDefHitTypeProtection(ALife::eHitTypeWound);
		radi_value += helmet->GetDefHitTypeProtection(ALife::eHitTypeRadiation);
        tele_value += helmet->GetDefHitTypeProtection(ALife::eHitTypeTelepatic);
		cmbn_value += helmet->GetDefHitTypeProtection(ALife::eHitTypeChemicalBurn);
		expl_value += helmet->GetDefHitTypeProtection(ALife::eHitTypeExplosion);
		fwou_value += helmet->GetDefHitTypeProtection(ALife::eHitTypeFireWound);
	}
	    // Kondr48: ������ �� ����������
	    burn_value += pActor->GetProtection_ArtefactsOnBelt(ALife::eHitTypeBurn);
		strk_value += pActor->GetProtection_ArtefactsOnBelt(ALife::eHitTypeStrike);
        shoc_value += pActor->GetProtection_ArtefactsOnBelt(ALife::eHitTypeShock); 
        woun_value += pActor->GetProtection_ArtefactsOnBelt(ALife::eHitTypeWound);
		radi_value += pActor->GetProtection_ArtefactsOnBelt(ALife::eHitTypeRadiation);
        tele_value += pActor->GetProtection_ArtefactsOnBelt(ALife::eHitTypeTelepatic);
		cmbn_value += pActor->GetProtection_ArtefactsOnBelt(ALife::eHitTypeChemicalBurn);
		expl_value += pActor->GetProtection_ArtefactsOnBelt(ALife::eHitTypeExplosion);
		fwou_value += pActor->GetProtection_ArtefactsOnBelt(ALife::eHitTypeFireWound);

	UIProgressBarBurnImmunity->SetProgressPos     (iCeil(burn_value*15.0f)/15.0f);
	UIProgressBarStrikeImmunity->SetProgressPos   (iCeil(strk_value*15.0f)/15.0f);
	UIProgressBarShockImmunity->SetProgressPos    (iCeil(shoc_value*15.0f)/15.0f);
	UIProgressBarWoundImmunity->SetProgressPos    (iCeil(woun_value*15.0f)/15.0f);
	UIProgressBarRadiationImmunity->SetProgressPos(iCeil(radi_value*15.0f)/15.0f);
	UIProgressBarTelepaticImmunity->SetProgressPos(iCeil(tele_value*15.0f)/15.0f);
	UIProgressBarChemicalImmunity->SetProgressPos (iCeil(cmbn_value*15.0f)/15.0f);
	UIProgressBarExplosionImmunity->SetProgressPos(iCeil(expl_value*15.0f)/15.0f);
	UIProgressBarFireWoundImmunity->SetProgressPos(iCeil(fwou_value*15.0f)/15.0f);
}