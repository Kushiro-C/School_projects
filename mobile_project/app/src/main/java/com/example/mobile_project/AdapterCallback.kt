package com.example.mobile_project

/**
 * Interface implemented by Activities
 * launching a RecyclerView
 * It is used to call a method of the parent Activity
 * from the RecyclerView adapter
 */
interface AdapterCallback {
    fun updateCallback(plant : Plant, pos : Int) // Method to implement in parent Activity
}