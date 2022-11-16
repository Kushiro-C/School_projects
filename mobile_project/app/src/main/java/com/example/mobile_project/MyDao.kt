package com.example.mobile_project

import androidx.room.*

// Data Access Object

@Dao
interface MyDao {
    // Queries to access database

    /**
     * insert a plant
     */
    @Insert(onConflict = OnConflictStrategy.IGNORE)
    fun insertPlant(vararg plant: Plant) : List<Long>

    /**
     * load all plants
     */
    @Query("SELECT * FROM Plant")
    fun loadAll(): Array<Plant>

    // Should not concat string with '||' in query, but add '%' to :nom
    //TODO : add latin search
    /**
     * search plants
     */
    @Query( "SELECT * FROM Plant where nom LIKE :nom || '%' OR latin LIKE :nom || '%'")
    fun loadPartialName( nom : String ) : Array<Plant> //TODO : add search with latin

    /**
     * update a plant
     */
    @Update
    fun updatePlant(vararg plant: Plant) : Int

    /**
     * Delete a plant with the given ID
     */
    @Query("DELETE FROM Plant WHERE idPlant = :idPlant")
    fun deletePlant(idPlant : Int)
}

/*
@Dao
abstract class TimestampDataDao {
    @Insert(onConflict = OnConflictStrategy.REPLACE)
    abstract fun insert(plant : Plant)

    fun insertWithTimestamp(plant: Plant) {
        insert(plant.apply{
            last_watering = System.currentTimeMillis()
        })
    }

    @Update
    abstract fun update(plant : Plant)

    fun updateWithTimestamp(plant: Plant) {
        insert(plant.apply{
            last_watering = System.currentTimeMillis()
        })
    }
}*/