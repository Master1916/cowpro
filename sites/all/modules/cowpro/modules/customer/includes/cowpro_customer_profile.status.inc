<?php
function _cowpro_customer_user_view($account, $view_mode) {
	global $user;
	if (user_access('Administer customer profiles') || ($user->uid == $account->uid) || _cowpro_customer_user_relationship_check($user->uid, $account->uid)) {
		$account->content['customer_profile'] = array(
				'#type' => 'user_profile_category',
				'#title' => '个人资料',
				'#weight' => 0,
		);
		$profile_types = cowpro_customer_profile_types();
		foreach($profile_types as $key => $value) {
			_cowpro_customer_user_view_detail($account, $value);
		}

	}
}

function _cowpro_customer_user_view_detail($account, $profile_type) {
	$type = $profile_type['type'];
	$name = $profile_type['name'];
	$weight = $profile_type['weight'];
	$verified = FALSE;
	$profile = cowpro_customer_profile_load_one_row_with_conditions($account->uid, $type);
	if ($profile) {
		$verified = $profile->verified;
	}

	$uid = $account->uid;
	$href = url("user/$uid/customer-profiles/$type");

	$callback = $type . '_user_view_detail';
	if (function_exists ( $callback )) {
		$markup = $callback($profile);
	} else {
		if ($verified) {
			$markup = '已认证';
		} else {
			$markup = '末认证';
		}
	}
	$markup = "<a href=$href>$markup</a>";
	$account->content['customer_profile'][$type] = array(
			'#type' => 'user_profile_item',
			'#title' => $name,
			'#markup' => $markup,
			'#weight' =>$weight,
	);
}

/**
 * 检查 $uid 是否是 $requestee_id 的朋友关系
 * @param unknown $uid
 * @param unknown $requestee_id
 * @return boolean
 */
function _cowpro_customer_user_relationship_check($uid, $requestee_id) {
	$relationship_type = user_relationships_type_load(array('name'=>'朋友'));
	$relationships = user_relationships_load(array(
			'requester_id' => $uid,
			'rtid'    => $relationship_type->rtid,
	));

	$relationship = array_shift($relationships);
	if (! $relationship) {
		return FALSE;
	}
	$uid_referree = $relationship->requestee_id;
	return $uid_referree == $requestee_id;
}
