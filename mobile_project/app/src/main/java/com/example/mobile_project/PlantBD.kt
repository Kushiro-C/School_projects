package com.example.mobile_project

import android.content.Context
import androidx.room.Database
import androidx.room.Room
import androidx.room.RoomDatabase

@Database(entities = [Plant::class], version = 6)
abstract class PlantDatabase : RoomDatabase() {
    abstract fun myDao(): MyDao

    companion object {
        @Volatile
        private var INSTANCE: PlantDatabase? = null

        fun getDatabase( context : Context ): PlantDatabase {
            if( INSTANCE != null )
                return INSTANCE!!
            val db = Room.databaseBuilder( context.applicationContext, PlantDatabase::class.java , "plant").build()
            INSTANCE = db
            return INSTANCE!!
        }
    }
}