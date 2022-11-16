package com.example.mobile_project

import androidx.fragment.app.Fragment
import androidx.fragment.app.FragmentActivity
import androidx.viewpager2.adapter.FragmentStateAdapter

class ScreenSlidePagerAdapter(
    fa : FragmentActivity,
    private var fragmentList : MutableList<Fragment>
    ) : FragmentStateAdapter( fa ) {

    override fun getItemCount(): Int = fragmentList.size
    override fun createFragment(position: Int): Fragment = fragmentList[position]
}