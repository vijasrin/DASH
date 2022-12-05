#include <iostream>
#include <vector>
#include <unordered_map>
#include <string.h>
extern "C" {
#include <sai.h>
}
extern sai_dash_direction_lookup_api_t sai_dash_direction_lookup_api_impl;
extern sai_dash_eni_api_t sai_dash_eni_api_impl;
extern sai_dash_acl_api_t sai_dash_acl_api_impl;
extern sai_dash_vnet_api_t sai_dash_vnet_api_impl;
extern sai_dash_outbound_ca_to_pa_api_t sai_dash_outbound_ca_to_pa_api_impl;

extern sai_status_t sai_create_direction_lookup_entry(
        _In_ const sai_direction_lookup_entry_t *direction_lookup_entry,
        _In_ uint32_t attr_count,
        _In_ const sai_attribute_t *attr_list);
extern sai_status_t sai_remove_direction_lookup_entry(
        _In_ const sai_direction_lookup_entry_t *direction_lookup_entry);

extern sai_status_t sai_create_eni_ether_address_map_entry(
        _In_ const sai_eni_ether_address_map_entry_t *outbound_eni_lookup_from_vm_entry,
        _In_ uint32_t attr_count,
        _In_ const sai_attribute_t *attr_list);
extern sai_status_t sai_remove_eni_ether_address_map_entry(
        _In_ const sai_eni_ether_address_map_entry_t *outbound_eni_lookup_from_vm_entry);

extern sai_status_t sai_create_vnet(
        _Out_ sai_object_id_t *vnet_id,
        _In_ sai_object_id_t switch_id,
        _In_ uint32_t attr_count,
        _In_ const sai_attribute_t *attr_list);
extern sai_status_t sai_remove_vnet(_In_ sai_object_id_t vnet_id);

extern sai_status_t sai_create_eni(
        _Out_ sai_object_id_t *eni_id,
        _In_ sai_object_id_t switch_id,
        _In_ uint32_t attr_count,
        _In_ const sai_attribute_t *attr_list);

extern sai_status_t sai_create_eni(
        _Out_ sai_object_id_t *eni_id,
        _In_ sai_object_id_t switch_id,
        _In_ uint32_t attr_count,
        _In_ const sai_attribute_t *attr_list);
extern sai_status_t sai_remove_eni(
        _In_ sai_object_id_t eni_id);

extern sai_status_t sai_create_dash_acl_group(
        _Out_ sai_object_id_t *acl_group_id,
        _In_ sai_object_id_t switch_id,
        _In_ uint32_t attr_count,
        _In_ const sai_attribute_t *attr_list);
extern sai_status_t sai_remove_dash_acl_group(
        _In_ sai_object_id_t eni_id);

int main(int argc, char **argv)
{
    sai_api_initialize(0UL, nullptr);

    sai_object_id_t switch_id = SAI_NULL_OBJECT_ID;
    sai_attribute_t attr;
    std::vector<sai_attribute_t> attrs;
    sai_object_id_t in_acl_group_id = SAI_NULL_OBJECT_ID;
    sai_object_id_t out_acl_group_id = SAI_NULL_OBJECT_ID;
    sai_object_id_t eni_id, eni_id_2;
    sai_object_id_t vnet_id;
    std::vector<uint32_t> bulk_attrs_count;
    std::vector<sai_status_t> bulk_status;

    sai_direction_lookup_entry_t dle = {};
    dle.switch_id = switch_id;
    dle.vni = 60;

    attr.id = SAI_DIRECTION_LOOKUP_ENTRY_ATTR_ACTION;
    attr.value.u32 = SAI_DIRECTION_LOOKUP_ENTRY_ACTION_SET_OUTBOUND_DIRECTION;
    attrs.push_back(attr);

    sai_status_t status = sai_dash_direction_lookup_api_impl.create_direction_lookup_entry(&dle, attrs.size(), attrs.data());
    if (status != SAI_STATUS_SUCCESS)
    {
        std::cout << "Failed to create Direction Lookup Entry" << std::endl;
        return 1;
    }

    attrs.clear();

    attr.id = SAI_DASH_ACL_GROUP_ATTR_IP_ADDR_FAMILY;
    attr.value.s32 = SAI_IP_ADDR_FAMILY_IPV4;
    attrs.push_back(attr);

    status = sai_dash_acl_api_impl.create_dash_acl_group(&in_acl_group_id, switch_id, attrs.size(), attrs.data());
    if (status != SAI_STATUS_SUCCESS)
    {
        std::cout << "Failed to create inbound Dash ACL group" << std::endl;
    }
    status = sai_dash_acl_api_impl.create_dash_acl_group(&out_acl_group_id, switch_id, attrs.size(), attrs.data());
    if (status != SAI_STATUS_SUCCESS)
    {
        std::cout << "Failed to create outbound Dash ACL group" << std::endl;
    }

    attrs.clear();

    attr.id = SAI_VNET_ATTR_VNI;
    attr.value.u32 = 9;
    attrs.push_back(attr);

    status = sai_dash_vnet_api_impl.create_vnet(&vnet_id, switch_id, attrs.size(), attrs.data());
    if (status != SAI_STATUS_SUCCESS)
    {
        std::cout << "Failed to create VNET table entry" << std::endl;
        return 1;
    }

    attrs.clear();

    attr.id = SAI_ENI_ATTR_CPS;
    attr.value.u32 = 10000;
    attrs.push_back(attr);

    attr.id = SAI_ENI_ATTR_PPS;
    attr.value.u32 = 100000;
    attrs.push_back(attr);

    attr.id = SAI_ENI_ATTR_FLOWS;
    attr.value.u32 = 100000;
    attrs.push_back(attr);

    attr.id = SAI_ENI_ATTR_ADMIN_STATE;
    attr.value.booldata = true;
    attrs.push_back(attr);

    attr.id = SAI_ENI_ATTR_VM_UNDERLAY_DIP;
    sai_ip_addr_t u_dip_addr = {.ip4 = 0xac010310};
    sai_ip_address_t u_dip = {.addr_family = SAI_IP_ADDR_FAMILY_IPV4,
                              .addr = u_dip_addr};
    attr.value.ipaddr = u_dip;
    attrs.push_back(attr);

    attr.id = SAI_ENI_ATTR_VM_VNI;
    attr.value.u32 = 9;
    attrs.push_back(attr);

    attr.id = SAI_ENI_ATTR_VNET_ID;
    attr.value.u32 = vnet_id;
    attrs.push_back(attr);

    std::unordered_map<uint32_t, uint16_t> acl_group_ids = {
      {SAI_ENI_ATTR_INBOUND_V4_STAGE1_DASH_ACL_GROUP_ID, in_acl_group_id},
      {SAI_ENI_ATTR_INBOUND_V4_STAGE2_DASH_ACL_GROUP_ID, in_acl_group_id},
      {SAI_ENI_ATTR_INBOUND_V4_STAGE3_DASH_ACL_GROUP_ID, in_acl_group_id},
      {SAI_ENI_ATTR_INBOUND_V4_STAGE4_DASH_ACL_GROUP_ID, in_acl_group_id},
      {SAI_ENI_ATTR_INBOUND_V4_STAGE5_DASH_ACL_GROUP_ID, in_acl_group_id},
      {SAI_ENI_ATTR_OUTBOUND_V4_STAGE1_DASH_ACL_GROUP_ID, out_acl_group_id},
      {SAI_ENI_ATTR_OUTBOUND_V4_STAGE2_DASH_ACL_GROUP_ID, out_acl_group_id},
      {SAI_ENI_ATTR_OUTBOUND_V4_STAGE3_DASH_ACL_GROUP_ID, out_acl_group_id},
      {SAI_ENI_ATTR_OUTBOUND_V4_STAGE4_DASH_ACL_GROUP_ID, out_acl_group_id},
      {SAI_ENI_ATTR_OUTBOUND_V4_STAGE5_DASH_ACL_GROUP_ID, out_acl_group_id},
      {SAI_ENI_ATTR_INBOUND_V6_STAGE1_DASH_ACL_GROUP_ID, SAI_NULL_OBJECT_ID},
      {SAI_ENI_ATTR_INBOUND_V6_STAGE2_DASH_ACL_GROUP_ID, SAI_NULL_OBJECT_ID},
      {SAI_ENI_ATTR_INBOUND_V6_STAGE3_DASH_ACL_GROUP_ID, SAI_NULL_OBJECT_ID},
      {SAI_ENI_ATTR_INBOUND_V6_STAGE4_DASH_ACL_GROUP_ID, SAI_NULL_OBJECT_ID},
      {SAI_ENI_ATTR_INBOUND_V6_STAGE5_DASH_ACL_GROUP_ID, SAI_NULL_OBJECT_ID},
      {SAI_ENI_ATTR_OUTBOUND_V6_STAGE1_DASH_ACL_GROUP_ID, SAI_NULL_OBJECT_ID},
      {SAI_ENI_ATTR_OUTBOUND_V6_STAGE2_DASH_ACL_GROUP_ID, SAI_NULL_OBJECT_ID},
      {SAI_ENI_ATTR_OUTBOUND_V6_STAGE3_DASH_ACL_GROUP_ID, SAI_NULL_OBJECT_ID},
      {SAI_ENI_ATTR_OUTBOUND_V6_STAGE4_DASH_ACL_GROUP_ID, SAI_NULL_OBJECT_ID},
      {SAI_ENI_ATTR_OUTBOUND_V6_STAGE5_DASH_ACL_GROUP_ID, SAI_NULL_OBJECT_ID},
    };
    for (const auto& acl_grp_pair : acl_group_ids) {
        attr.id = acl_grp_pair.first;
        attr.value.oid = acl_grp_pair.second;
        attrs.push_back(attr);
    }

    status = sai_dash_eni_api_impl.create_eni(&eni_id, switch_id, attrs.size(), attrs.data());
    if (status != SAI_STATUS_SUCCESS)
    {
        std::cout << "Failed to create ENI object" << std::endl;
        return 1;
    }

    attrs.clear();

    attr.id = SAI_ENI_ATTR_CPS;
    attr.value.u32 = 10000;
    attrs.push_back(attr);

    attr.id = SAI_ENI_ATTR_PPS;
    attr.value.u32 = 100000;
    attrs.push_back(attr);

    attr.id = SAI_ENI_ATTR_FLOWS;
    attr.value.u32 = 100000;
    attrs.push_back(attr);

    attr.id = SAI_ENI_ATTR_ADMIN_STATE;
    attr.value.booldata = true;
    attrs.push_back(attr);

    attr.id = SAI_ENI_ATTR_VM_UNDERLAY_DIP;
    attr.value.ipaddr = u_dip;
    attrs.push_back(attr);

    attr.id = SAI_ENI_ATTR_VM_VNI;
    attr.value.u32 = 9;
    attrs.push_back(attr);

    attr.id = SAI_ENI_ATTR_VNET_ID;
    attr.value.u32 = vnet_id;
    attrs.push_back(attr);

    for (const auto& acl_grp_pair : acl_group_ids) {
        attr.id = acl_grp_pair.first;
        attr.value.oid = acl_grp_pair.second;
        attrs.push_back(attr);
    }

    status = sai_dash_eni_api_impl.create_eni(&eni_id_2, switch_id, attrs.size(), attrs.data());
    if (status != SAI_STATUS_SUCCESS)
    {
        std::cout << "Failed to create ENI object" << std::endl;
        return 1;
    }

    attrs.clear();

    sai_eni_ether_address_map_entry_t eam;
    eam.switch_id = switch_id;
    eam.address[0] = 0xaa;
    eam.address[1] = 0xcc;
    eam.address[2] = 0xcc;
    eam.address[3] = 0xcc;
    eam.address[4] = 0xcc;
    eam.address[5] = 0xcc;

    attr.id = SAI_ENI_ETHER_ADDRESS_MAP_ENTRY_ATTR_ENI_ID;
    attr.value.oid = eni_id;
    attrs.push_back(attr);

    status = sai_dash_eni_api_impl.create_eni_ether_address_map_entry(&eam, attrs.size(), attrs.data());
    if (status != SAI_STATUS_SUCCESS)
    {
        std::cout << "Failed to create ENI Lookup From VM" << std::endl;
        return 1;
    }

    sai_eni_ether_address_map_entry_t eam2;
    eam2.switch_id = switch_id;
    eam2.address[0] = 0xdd;
    eam2.address[1] = 0xee;
    eam2.address[2] = 0xee;
    eam2.address[3] = 0xee;
    eam2.address[4] = 0xee;
    eam2.address[5] = 0xee;

    attr.id = SAI_ENI_ETHER_ADDRESS_MAP_ENTRY_ATTR_ENI_ID;
    attr.value.oid = eni_id_2;
    attrs.push_back(attr);

    status = sai_dash_eni_api_impl.create_eni_ether_address_map_entry(&eam2, attrs.size(), attrs.data());
    if (status != SAI_STATUS_SUCCESS)
    {
        std::cout << "Failed to create ENI Lookup From VM" << std::endl;
        return 1;
    }

    // mapping bulk create
    auto ca_pa_bulk_count = 10;
    std::vector<sai_outbound_ca_to_pa_entry_t> ca_pa_bulk_keys;
    std::vector<std::vector<sai_attribute_t>> ca_pa_bulk_attrs_list;
    std::vector<const sai_attribute_t*> ca_pa_bulk_attrs;
    bulk_status.resize(ca_pa_bulk_count);
    sai_ip_addr_t ca_addr4 = {.ip4 = 0x0a080808};
    sai_ip_addr_t pa_addr4 = {.ip4 = 0xad030110};

    for (auto i = 0; i < ca_pa_bulk_count; ++i) {
    sai_outbound_ca_to_pa_entry_t ca_pa;
    ca_pa.switch_id = switch_id;
    ca_pa.dst_vnet_id = vnet_id;
    ca_addr4.ip4 += i;
    sai_ip_address_t ca_addr = {.addr_family = SAI_IP_ADDR_FAMILY_IPV4,
                              .addr = ca_addr4};
    ca_pa.dip = ca_addr;
    ca_pa_bulk_keys.push_back(ca_pa);

    attrs.clear();

    attr.id = SAI_OUTBOUND_CA_TO_PA_ENTRY_ATTR_OVERLAY_DMAC;
    attr.value.mac[0] = 0xdd;
    attr.value.mac[1] = 0xee;
    attr.value.mac[2] = 0xee;
    attr.value.mac[3] = 0xee;
    attr.value.mac[4] = 0xee;
    attr.value.mac[5] = 0x1;
    attr.value.mac[5] += i;
    attrs.push_back(attr);

    attr.id = SAI_OUTBOUND_CA_TO_PA_ENTRY_ATTR_UNDERLAY_DIP;
    if (i % 2 == 0) {
        pa_addr4.ip4 += i;
    }
    sai_ip_address_t pa_addr = {.addr_family = SAI_IP_ADDR_FAMILY_IPV4,
                              .addr = pa_addr4};
    attr.value.ipaddr = pa_addr;
    attrs.push_back(attr);

    attr.id = SAI_OUTBOUND_CA_TO_PA_ENTRY_ATTR_USE_DST_VNET_VNI;
    attr.value.booldata = true;
    attrs.push_back(attr);

    ca_pa_bulk_attrs_list.push_back(attrs);
    ca_pa_bulk_attrs.push_back(ca_pa_bulk_attrs_list.back().data());
    bulk_attrs_count.push_back(attrs.size());
    }

    status = sai_dash_outbound_ca_to_pa_api_impl.create_outbound_ca_to_pa_entries(ca_pa_bulk_count,
                                                                             ca_pa_bulk_keys.data(),
                                                                             bulk_attrs_count.data(),
                                                                             ca_pa_bulk_attrs.data(),
                                                                             SAI_BULK_OP_ERROR_MODE_STOP_ON_ERROR,
                                                                             bulk_status.data());
    if (status != SAI_STATUS_SUCCESS)
    {
        std::cout << "Failed to create outbound CA to PA entry" << std::endl;
        return 1;
    }
    attrs.clear();
    bulk_status.clear();
    bulk_attrs_count.clear();

    std::cout << "Press key + enter to update ENI";
    char c;
    std::cin >> c ;

    attr.id = SAI_ENI_ATTR_VM_UNDERLAY_DIP;
    u_dip.addr.ip4 = 0xae010310;
    attr.value.ipaddr = u_dip;
    attrs.push_back(attr);

    status = sai_dash_eni_api_impl.set_eni_attribute(eni_id_2, &attrs[0]);
    if (status != SAI_STATUS_SUCCESS)
    {
        std::cout << "Failed to create ENI object" << std::endl;
        return 1;
    }

    attrs.clear();

    std::cout << "Press key + enter to update ENI";
    std::cin >> c ;

    attr.id = SAI_ENI_ATTR_VM_UNDERLAY_DIP;
    u_dip.addr.ip4 = 0xac010310;
    attr.value.ipaddr = u_dip;
    attrs.push_back(attr);

    status = sai_dash_eni_api_impl.set_eni_attribute(eni_id_2, &attrs[0]);
    if (status != SAI_STATUS_SUCCESS)
    {
        std::cout << "Failed to create ENI object" << std::endl;
        return 1;
    }

    attrs.clear();

    std::cout << "Press key + enter to delete all";
    std::cin >> c ;
    // Delete everything in reverse order

    status = sai_dash_outbound_ca_to_pa_api_impl.remove_outbound_ca_to_pa_entries(ca_pa_bulk_count,
                                                                                  ca_pa_bulk_keys.data(),
                                                                                  SAI_BULK_OP_ERROR_MODE_STOP_ON_ERROR,
                                                                                  bulk_status.data());
    if (status != SAI_STATUS_SUCCESS)
    {
        std::cout << "Failed to remove ENI Lookup From VM" << std::endl;
        return 1;
    }

    status = sai_dash_eni_api_impl.remove_eni_ether_address_map_entry(&eam2);
    if (status != SAI_STATUS_SUCCESS)
    {
        std::cout << "Failed to remove ENI Lookup From VM" << std::endl;
        return 1;
    }

    status = sai_dash_eni_api_impl.remove_eni_ether_address_map_entry(&eam);
    if (status != SAI_STATUS_SUCCESS)
    {
        std::cout << "Failed to remove ENI Lookup From VM" << std::endl;
        return 1;
    }

    status = sai_dash_eni_api_impl.remove_eni(eni_id_2);
    if (status != SAI_STATUS_SUCCESS)
    {
        std::cout << "Failed to remove ENI object " << eni_id << std::endl;
        return 1;
    }

    status = sai_dash_eni_api_impl.remove_eni(eni_id);
    if (status != SAI_STATUS_SUCCESS)
    {
        std::cout << "Failed to remove ENI object " << eni_id << std::endl;
        return 1;
    }

    status = sai_dash_vnet_api_impl.remove_vnet(vnet_id);
    if (status != SAI_STATUS_SUCCESS)
    {
        std::cout << "Failed to remove VNET table entry" << std::endl;
        return 1;
    }
    status = sai_dash_acl_api_impl.remove_dash_acl_group(out_acl_group_id);
    if (status != SAI_STATUS_SUCCESS)
    {
        std::cout << "Failed to remove Outbound ACL group object "
                  << out_acl_group_id << std::endl;
        return 1;
    }
    status = sai_dash_acl_api_impl.remove_dash_acl_group(in_acl_group_id);
    if (status != SAI_STATUS_SUCCESS)
    {
        std::cout << "Failed to remove Inbound ACL group object "
                  << in_acl_group_id << std::endl;
        return 1;
    }

    status = sai_dash_direction_lookup_api_impl.remove_direction_lookup_entry(&dle);
    if (status != SAI_STATUS_SUCCESS)
    {
        std::cout << "Failed to remove Direction Lookup Entry" << std::endl;
        return 1;
    }

    std::cout << "Done." << std::endl;

    return 0;
}

